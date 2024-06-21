# app.py

import RPi.GPIO as GPIO
import os
from threading import Thread
from flask import Flask, render_template, request, redirect, session, Response
from linuxpy.video.device import Device, VideoCapture
import time
import math
app = Flask(__name__)


@app.route('/')
def controls():
    return render_template('index.html')


def get_file(filename):  # pragma: no cover
    try:
        # Figure out how flask returns static files
        # Tried:
        # - render_template
        # - send_file
        # This should not be so non-obvious
        return open(filename).read()
    except IOError as exc:
        return str(exc)


@app.route('/', defaults={'path': ''})
@app.route('/<path:path>')
def get_resource(path):  # pragma: no cover
    mimetypes = {
        ".css": "text/css",
        ".html": "text/html",
        ".js": "application/javascript",
    }
    complete_path = os.path.join('static', path)
    ext = os.path.splitext(path)[1]
    mimetype = mimetypes.get(ext, "text/html")
    content = get_file(complete_path)
    return Response(content, mimetype=mimetype)


FORWARD = 0
BACKWARD = 1
LEFT = 2
RIGHT = 3
states = [False, False, False, False]
dir_name = ["forward", "backward", "left", "right"]
key_dir_name = ["ArrowUp", "ArrowDown", "ArrowLeft", "ArrowRight"]

motor_states = [ 
    # FORWARD FALSE
    [
        # BACKWARD FALSE
        [
            # LEFT FALSE
            [
                # RIGHT FALSE
                [
                   0, 0, 0, 0
                ],
                # RIGHT TRUE
                [
                   1, 0, 0, 0
                ],
            ],
            # LEFT TRUE
            [
                # RIGHT FALSE
                [
                   0, 0, 1, 0
                ],
                # RIGHT TRUE
                [
                   0, 0, 0, 0
                ],
            ],
       ],
        # BACKWARD TRUE
        [
            # LEFT FALSE
            [
                # RIGHT FALSE
                [
                   0, 1, 0, 1
                ],
                # RIGHT TRUE
                [
                   0, 1, 0, 0
                ],
            ],
            # LEFT TRUE
            [
                # RIGHT FALSE
                [
                   0, 0, 0, 1
                ],
                # RIGHT TRUE
                [
                   0, 1, 0, 1
                ],
            ],
        ],
    ],
    # FORWARD TRUE
    [
        # BACKWARD FALSE
        [
            # LEFT FALSE
            [
                # RIGHT FALSE
                [
                   1, 0, 1, 0
                ],
                # RIGHT TRUE
                [
                   1, 0, 0, 0
                ],
            ],
            # LEFT TRUE
            [
                # RIGHT FALSE
                [
                   0, 0, 1, 0
                ],
                # RIGHT TRUE
                [
                   1, 0, 1, 0
                ],
            ],
        ],
        # BACKWARD TRUE
        [
            # LEFT FALSE
            [
                # RIGHT FALSE
                [
                   0, 0, 0, 0
                ],
                # RIGHT TRUE
                [
                   0, 0, 0, 0
                ],
            ],
            # LEFT TRUE
            [
                # RIGHT FALSE
                [
                   0, 0, 0, 0
                ],
                # RIGHT TRUE
                [
                   0, 0, 0, 0
                ],
            ],
        ],
    ],
]

def move(dir):
    states[dir] = not states[dir]

    state = motor_states[int(states[FORWARD])][int(states[BACKWARD])][int(states[LEFT])][int(states[RIGHT])]
    setMotors(state)
    if states[dir]:
        return render_template('button.html', eventType='keyup', key=key_dir_name[dir], dir=dir_name[dir])
    else:
        return render_template('button.html', eventType='keydown', key=key_dir_name[dir], dir=dir_name[dir])



@app.route('/move-forward', methods=['PUT'])
def move_forward():
    return move(FORWARD)


@app.route('/move-backward', methods=['PUT'])
def move_backward():
    return move(BACKWARD)


@app.route('/move-left', methods=['PUT'])
def move_left():
    return move(LEFT)


@app.route('/move-right', methods=['PUT'])
def move_right():
    return move(RIGHT)

@app.route('/sensors', methods=['PUT'])
def sensors_text():
    readings = readSensors(sensors)
    offset = readings[0]-readings[1]        
    angle = math.atan(offset/45)*180/math.pi

    return Response("<p hx-trigger=\"every 1s\" hx-swap=\"outerHTML\" hx-put=\"/sensors\">Front Right Sensor: " + str(readings[0]) + "<br>Rear Right Sensor: " + str(readings[1]) + "<br>Angle: " + str(angle) + " degrees</p>", mimetype="text/html")
		
@app.route('/automate', methods=['PUT'])
def automate():
    setMotors([0, 0, 0, 0])
    global automate_mode
    automate_mode = True

    print("Automated mode turned on")
    return Response("<button hx-trigger=\"click\" style=\"font-size: 20px;\" hx-swap=\"outerHTML\" hx-put=\"/manual\">Manual</button>", mimetype='text/html')

@app.route('/manual', methods=['PUT'])
def manual():
    global automate_mode
    automate_mode = False
    
    print("Manual mode turned on")
    setMotors([0, 0, 0, 0])

    return Response("<button hx-trigger=\"click\" style=\"font-size: 20px;\" hx-swap=\"outerHTML\" hx-put=\"/automate\">Automate</button>", mimetype='text/html')

def gen():
    """Video streaming generator function."""
    with Device("/dev/video0") as cam:
        capture = VideoCapture(cam)
        capture.set_format(640, 480, "MJPG")
        with capture:
            for frame in capture:
                yield b"--frame\r\nContent-Type: image/jpeg\r\n\r\n" + frame.data + b"\r\n"

@app.route('/video_feed')
def video_feed():
    """Video streaming route. Put this in the src attribute of an img tag."""
    return Response(gen(),
                    mimetype='multipart/x-mixed-replace; boundary=frame')


class Arrows:
    def __init__(self, forward, backward, left, right):
        self.forward = forward
        self.backward = backward
        self.left = left
        self.right = right

    def setup(self):
        pass


class Motor:
    
    def __init__(self, en, in1, in2):
        self.en = en
        self.in1 = in1
        self.in2 = in2

    def setSpeed(self, speed):
        self.PWM.ChangeDutyCycle(speed)

    def setup(self):
        GPIO.setup(self.en, GPIO.OUT)
        self.PWM = GPIO.PWM(self.en, 100)
        self.PWM.start(0)
        #self.PWM.ChangeDutyCycle(100)
        self.setSpeed(100)

        GPIO.setup(self.in1, GPIO.OUT)
        GPIO.setup(self.in2, GPIO.OUT)
        

        #GPIO.output(self.in1, 1)

class Record:
    def __init__(self):
        self.history = []
        self.average = 0

    def add(self, value):
        self.history.insert(0, value)
        self.average = (self.average*(len(self.history)-1)+value)/len(self.history)
        if (len(self.history) > 100):
            temp = self.history.pop() 
            self.average =(self.average*(len(self.history)+1)-temp)/len(self.history)

    def reset(self, value):
        self.average = value+20
        self.history = [value]*100

class Sensor:
    def __init__(self, trig, echo):
        self.trig = trig
        self.echo = echo

    def setup(self):
        GPIO.setup(self.trig, GPIO.OUT)
        GPIO.setup(self.echo, GPIO.IN)

    def read(self):
        GPIO.output(self.trig, True)
        time.sleep(0.00001)
        GPIO.output(self.trig, False)


        stop_time = start_time = time_out = time.time()
        while GPIO.input(self.echo) == 0:
            start_time = time.time()
        while GPIO.input(self.echo) == 1:
            stop_time = time.time()

        return 34300 * (stop_time - start_time)/2



motors = [Motor(19, 23, 21), Motor(32, 24, 26)]
sensors = [Sensor(12, 11), Sensor(5, 3)]

def setMotors(state, left_speed=100, right_speed=100):
    GPIO.output(motors[0].in1, state[0])
    GPIO.output(motors[0].in2, state[1])
    GPIO.output(motors[1].in1, state[2])
    GPIO.output(motors[1].in2, state[3])

    motors[0].setSpeed(min(100, left_speed))
    motors[1].setSpeed(min(100, right_speed))


def readSensors(sensors, delay=0.01):
    readings = [0]*len(sensors)
    for i in range(len(sensors)):
        readings[i] = sensors[i].read()
        time.sleep(delay)

    return readings



distance_record = Record()
angle_record = Record()

def automation(angle_error=3, distance_error=4, target=50, delay=0.05, total_max_angle=30, door_threshold=10):
   
    global automate_mode
    while True:
        readings = readSensors(sensors, delay)

        offset = readings[0]-readings[1]
        
        angle = math.atan(offset/45)   
        
        actual_distance = math.sin(math.pi/2-abs(angle))*(readings[0]+readings[1])/2
        #actual_distance = math.sin(math.pi/2-abs(angle))*(readings[0])/2

        distance_record.add(actual_distance) 

        max_angle = total_max_angle*(abs(actual_distance-target)/80)

        print(distance_record.average, readings[0], readings[1], actual_distance, angle/3.14*180,  max_angle)

        correction_speed = 100
        correction_speed_angle = 100
        
        if actual_distance-target != 0:
            correction_speed = max(0, min(100, 100/abs(actual_distance-target) + 30))
        if offset != 0:
            correction_speed_angle = max(0, 80/abs(offset)+30-40*(angle*180/math.pi)/total_max_angle)
       
        if not automate_mode:
            continue

        if readings[0] < 800 and math.sin(math.pi/2-abs(angle))*readings[0] - distance_record.average > door_threshold:
            #setMotors([0,0,0,0])
            #time.sleep(3)
            #readings = readSensors(sensors, delay)
            #if (readings[0] - distance_record.average > door_threshold):
            setMotors([1,0,1,0], left_speed=80)
            #while readings[1] < 800 and readings[1] - distance_record.average > door_threshold:
             #   readings = readSensors(sensors, delay)

            time.sleep(5)

            setMotors([0,0,0,0])
            time.sleep(20)

            setMotors([1,0,1,0], left_speed=80)
            time.sleep(8)

            readings = readSensors(sensors, delay)

            offset = readings[0]-readings[1]
    
            angle = math.atan(offset/45)   
        
            actual_distance = math.sin(math.pi/2-abs(angle))*(readings[0]+readings[1])/2

            distance_record.reset(actual_distance)


        elif actual_distance-target > distance_error:
            if angle/math.pi*180 > -max_angle:
                setMotors([1,0,1,0], right_speed=correction_speed)
            elif angle/math.pi*180 + angle_error < -max_angle:
                setMotors([1,0,1,0], left_speed=correction_speed)

        elif actual_distance-target < -distance_error:
            if angle/math.pi*180 < max_angle:
                setMotors([1,0,1,0], left_speed=correction_speed)
            elif angle/math.pi*180 - angle_error > max_angle:
                setMotors([1,0,1,0], right_speed=correction_speed) 

        elif offset < -angle_error:
            setMotors([1,0,1,0], left_speed=correction_speed_angle)
        elif offset > angle_error:
            setMotors([1,0,1,0], right_speed=correction_speed_angle)
        else:
            setMotors([1,0,1,0])



def setup():
   
    GPIO.setmode(GPIO.BOARD)
    #GPIO.setmode(GPIO.BCM)
    
    for m in motors:
        m.setup()
    for s in sensors:
        s.setup()


if __name__ == '__main__':
    setup()
    setMotors([0, 0, 0, 0])
    
    global thread
    thread = Thread(target = automation, args = {})
    thread.start()

    automate_mode = False  

    app.run(debug=True, port=80, host="0.0.0.0")
    GPIO.cleanup()
