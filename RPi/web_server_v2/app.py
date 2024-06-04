# app.py

import RPi.GPIO as GPIO
import os
from flask import Flask, render_template, request, redirect, session, Response

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

    GPIO.output(motors[0].in1, state[0])
    GPIO.output(motors[0].in2, state[1])
    GPIO.output(motors[1].in1, state[2])
    GPIO.output(motors[1].in2, state[3])
    print(states)
    print(state)
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

    def setup(self):
        GPIO.setup(self.en, GPIO.OUT)
        self.PWM = GPIO.PWM(self.en, 100)
        self.PWM.start(0)
        self.PWM.ChangeDutyCycle(100)

        GPIO.setup(self.in1, GPIO.OUT)
        GPIO.setup(self.in2, GPIO.OUT)
        

        #GPIO.output(self.in1, 1)



motors = [Motor(19, 21, 23), Motor(32, 26, 24)]

def setup():
    
    GPIO.setmode(GPIO.BOARD)
    
    for m in motors:
        m.setup()


if __name__ == '__main__':
    setup()


    app.run(debug=True, port=443, host="0.0.0.0")
