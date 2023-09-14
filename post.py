#!/usr/bin/python

import os
import sys

# Get the content length from the environment variables
content_length = int(os.environ.get('CONTENT_LENGTH', 0))

# Read the POST data from stdin
post_data = sys.stdin.read(content_length)

# Now, post_data contains the request body data (e.g., form data or JSON)

print "<html>"
print "<body>"
print "<h1>POST Data Received</h1>"
print "<p>Data:</p>"
print "<pre>%s</pre>" % post_data
print "</body>"
print "</html>"
