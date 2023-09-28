#!/usr/bin/php

<?php

// Get the content length from the environment variables
$contentLength = (int) $_SERVER['CONTENT_LENGTH'];

// Read the POST data from stdin
$postData = fread(STDIN, $contentLength);

// Now, $postData contains the request body data (e.g., form data or JSON)

// Print the HTML response
echo "<html>\n";
echo "<body>\n";
echo "<h1 style=\"color: darkviolet;\">Data Received (CGI TEST)</h1>\n";
echo "<p>Hi: </p>\n";
echo "<pre>" . htmlspecialchars($postData) . "</pre>\n";
echo "</body>\n";
echo "</html>\n";

?>

