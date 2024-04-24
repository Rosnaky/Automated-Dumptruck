# app.py

import wiringPi
from flask import Flask, render_template, request, redirect, session

app = Flask(__name__)

# To render a login form 
@app.route('/')
def view_form():
    return render_template('index.html')

@app.route('/move-speed', methods=['PUT'])
def handle_put():
    print("Wiring")

if __name__ == '__main__':
    wiringPiSetupGpio()
    app.run()
