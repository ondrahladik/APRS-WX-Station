<?php
$servername = "YOUR_SERVER";               // DB server
$username = "YOUR_USERNAME";               // DB user
$password = "YOUR_PASSWORD";               // DB pass
$dbname = "YOUR_DBNAME";                   // DB name

// Create a connection
$conn = new mysqli($servername, $username, $password, $dbname);

// Check the connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

// Získání dat z požadavku
$temperature = isset($_GET['temperature']) ? floatval($_GET['temperature']) : 0;
$pressure = isset($_GET['pressure']) ? floatval($_GET['pressure']) : 0;
$humidity = isset($_GET['humidity']) ? floatval($_GET['humidity']) : 0;
$light = isset($_GET['light']) ? floatval($_GET['light']) : 0;
$co2_ppm = isset($_GET['co2_ppm']) ? floatval($_GET['co2_ppm']) : 0;
$tvoc_ppb = isset($_GET['tvoc_ppb']) ? floatval($_GET['tvoc_ppb']) : 0;

// Debugging: Check the received values
error_log("Received values - Temperature: $temperature, Pressure: $pressure, Humidity: $humidity, Light: $light, CO2: $co2_ppm, TVOC: $tvoc_ppb");

// Prepare the SQL query
$sql = "INSERT INTO wx_station (temperature, pressure, humidity, light, co2_ppm, tvoc_ppb, timestamp) VALUES (?, ?, ?, ?, ?, ?, NOW())";
$stmt = $conn->prepare($sql);

// Check if the statement was prepared successfully
if ($stmt === false) {
    die("Error preparing the SQL statement: " . $conn->error);
}

// Bind parameters
$stmt->bind_param("dddddd", $temperature, $pressure, $humidity, $light, $co2_ppm, $tvoc_ppb);

// Execute the query
if ($stmt->execute()) {
    echo "New record created successfully";
} else {
    echo "Error: " . $stmt->error;
}

// Close the connection
$stmt->close();
$conn->close();
?>
