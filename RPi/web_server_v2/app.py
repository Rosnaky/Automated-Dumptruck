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


@app.route('/move-forward', methods=['PUT'])
def handle_put():
    print("forward")
    return Response('', mimetype='text/txt')

if __name__ == '__main__':
    app.run(debug=True, port=80, host="0.0.0.0")
    
