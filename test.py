#!/usr/bin/python

# Import necessary libraries
import cgi

# Create an instance of the FieldStorage class to get data from the form
form = cgi.FieldStorage()

# Set the content type to HTML
# print("Content-type: text/html\n")

# HTML content
print("<html>")
print("<head>")
print("<title>Simple CGI Python Script</title>")
print("</head>")
print("<body>")
print("<h1>Hello, World!</h1>")
print("<p style=\"color:dodgerblue;\">This is a simple CGI Python script.</p>")

# Check if the "name" parameter is passed from a form
if "name" in form:
    name = form["name"].value
    print("<p>You entered: %s</p>" % name)

print("</body>")
print("</html>")
