#!/Users/mjlem/php-cgi
<?php

// Start a session or resume the current session
session_start();

// Check if the user wants to log out
if (isset($_GET['logout'])) {
    session_destroy();
    header('Location: ' . $_SERVER['PHP_SELF']);
    exit();
}

// Check if a user is already logged in
if (isset($_SESSION['user'])) {
    $username = $_SESSION['user'];
    echo "Welcome back, $username!";
    echo "<br>";
    echo '<a href="?logout=1">Logout</a>';
} else {
    // Check if the user is trying to log in
    if (isset($_POST['username']) && isset($_POST['password'])) {
        $enteredUsername = $_POST['username'];
        $enteredPassword = $_POST['password'];

        // In a real application, you would validate the username and password.
        // For this example, we'll use a simple check.
        if ($enteredUsername === 'user' && $enteredPassword === 'password') {
            // Set a session variable to mark the user as logged in
            $_SESSION['user'] = $enteredUsername;
            echo "Welcome, $enteredUsername!";
            echo "<br>";
            echo '<a href="?logout=1">Logout</a>';
        } else {
            echo "Invalid credentials. Please try again.";
            echo "<br>";
            echo '<a href="?login=1">Login</a>';
        }
    } else {
        // If the user is not logged in and no login attempt was made, show a login form
        if (isset($_GET['login'])) {
            echo '<form method="post" action="?login=1">';
            echo 'Username: <input type="text" name="username"><br>';
            echo 'Password: <input type="password" name="password"><br>';
            echo '<input type="submit" value="Login">';
            echo '</form>';
        } else {
            echo "You are not logged in.";
            echo "<br>";
            echo '<a href="?login=1">Login</a>';
        }
    }
}



?>
