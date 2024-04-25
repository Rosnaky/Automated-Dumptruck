# app.py

import wiringpi
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


UP = 0; DOWN = 1; LEFT = 2; RIGHT = 0;
states = [False, False, False, False]


@app.route('/move-forward', methods=['PUT'])
def move_forward():
    states[0] = not states[0]
    print("forward")
    if states[0]: 
        return render_template('buttons.html', eventType='keyUp', key='ArrowUp', dir='forward')
    else:
        return render_template('buttons.html', eventType='keyUp', key='ArrowUp', dir='forward')


@app.route('/move-backward', methods=['PUT'])
def move_backward():
    states[1] = not states[1]
    print("backward")
    return Response('', mimetype='text/txt')


@app.route('/move-left', methods=['PUT'])
def move_left():
    print("left")
    return Response('', mimetype='text/txt')


@app.route('/move-right', methods=['PUT'])
def move_right():
    print("right")
    return Response('', mimetype='text/txt')



if __name__ == '__main__':
    app.run(debug=True, port=80, host="0.0.0.0")
    

