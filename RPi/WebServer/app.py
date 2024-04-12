from flask import Flask, render_template
import pi

def create_app():
    app =  Flask(__name__)
    
    return app


app = create_app()
    
@app.route("/")
def index():
    return render_template("index.html")

@app.route("/led/on")
def led_on():
    print("LED ON")
    pi.client.publish("/leds/esp8266", "ON")

    return render_template("index.html")

@app.route("/led/off")
def led_off():
    print("LED OFF")
    pi.client.publish("/leds/esp8266", "OFF")

    return render_template("index.html")

@app.route("/led/toggle")
def led_toggle():
    print("LED TOGGLE")
    pi.client.publish("/leds/esp8266", "TOGGLE")

    return render_template("index.html")


if __name__ == "__main__":
    app.run(debug=True, port=80, host="0.0.0.0")