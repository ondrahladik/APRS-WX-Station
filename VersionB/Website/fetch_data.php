<?php
$servername = "YOUR_SERVER";               // 1. DB server
$username = "YOUR_USERNAME";               // 2. DB user
$password = "YOUR_PASSWORD";               // 3. DB pass
$dbname = "YOUR_DBNAME";                   // 4. DB name

// Create a connection
$conn = new mysqli($servername, $username, $password, $dbname);

// Check connection
if ($conn->connect_error) {
    die("Připojení selhalo: " . $conn->connect_error);
}

// SQL query to get the newest row
$sql = "SELECT temperature, pressure, humidity, light, co2_ppm, tvoc_ppb, timestamp FROM wx_station ORDER BY id DESC LIMIT 1";
$result = $conn->query($sql);

$data = array();

if ($result->num_rows > 0) {
    $row = $result->fetch_assoc();

    // Create a DateTime object from timestamp
    $dateTime = new DateTime($row['timestamp']);

    // Create a DateTime object from timestamp
    $formattedDate = $dateTime->format('d. m. Y H:i:s');

    // Format temperature and pressure to 2 decimal places
    $formattedTemperature = number_format((float)$row['temperature'], 2, '.', '');
    $formattedPressure = number_format((float)$row['pressure'], 2, '.', '');

    // Add formatted data to the array
    $data = array(
        'temperature' => $formattedTemperature,
        'pressure' => $formattedPressure,
        'humidity' => $row['humidity'],
        'light' => $row['light'],
        'co2_ppm' => $row['co2_ppm'],
        'tvoc_ppb' => $row['tvoc_ppb'],
        'timestamp' => $formattedDate
    );
}

echo json_encode($data);

$conn->close();
?>
