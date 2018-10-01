<!DOCTYPE html>
<html>
<head>
<title>Sample Website</title>
<style>
body {
    background: url("background.png");
    background-size: cover;
    font-family: "Lucida Sans Unicode", "Century Gothic", "Trebuchet MS", sans-serif;
    width: 50%;
    margin: 0 auto;
    margin-top: 80px;
    padding-top: 20px;
}

h1, h2, h3 {
    font-family: "Rockwell", "Warnock", "Georgia", serif;
}

#wrapper {
    background: rgba(223, 223, 223, 0.85);
    border: 1px solid rgba(0, 0, 0, 0.5);
    border-radius: 3px;
    box-shadow: 0px 0px 5px rgba(0, 0, 0, 0.5);
    margin: 16px;
    overflow: hidden;
}

#wrapper h1 {
    background: -moz-linear-gradient(rgba(255, 255, 255, 0.6), transparent);
    font-size: 28px;
    margin: 0;
    padding: 5px 0px 5px 10px;
}

#wrapper h2, #wrapper h3 {
    border-bottom: 2px solid rgba(127, 127, 127, 0.5);
    margin-left: 10px;
    margin-right: 20px;
}

#wrapper p, #wrapper form {
    margin: 15px;
}
</style>
</head>
<body>

<div id="wrapper">
    <h1>Sample Website</h1>

    <form action="/index.php" method="post">
        First Name: <input type="text" name="firstName" />
        <br />
        Last Name: <input type="text" name="lastName" />
        <br />
        <input type="submit" value="Submit" />
    </form>

    <p><a href="/info.php">View phpinfo()</a></p>
    <p><a href="/background.png">View background image</a></p>

    <p><i><?php echo date(DATE_RFC2822); ?></i></p>
</div>

</body>
</html>
