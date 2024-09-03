<?php
header('Content-Type: application/json');

$servername = "YOUR_SERVER";               // 1. DB server
$username = "YOUR_USERNAME";               // 2. DB user
$password = "YOUR_PASSWORD";               // 3. DB pass
$dbname = "YOUR_DBNAME";                   // 4. DB name

// Create a connection
$conn = new mysqli($servername, $username, $password, $dbname);

// Check connection
if ($conn->connect_error) {
    die("Chyba připojení k databázi: " . $conn->connect_error);
}

// Default time range and dataset 
$limit = 6;
$data = 'temp';

// Check for existence of parameters in URL
if (isset($_GET['time'])) {
    if ($_GET['time'] == '24h') {
        $limit = 144;
    } elseif ($_GET['time'] == '12h') {
        $limit = 72;
    } elseif ($_GET['time'] == '6h') {
        $limit = 36;
    }
}

if (isset($_GET['data']) && in_array($_GET['data'], ['temp', 'humi', 'pres', 'light', 'co2', 'tvoc'])) {
    $data = $_GET['data'];
}

// SQL query to get data by time range
$sql = "SELECT temperature, pressure, humidity, light, co2_ppm, tvoc_ppb, timestamp FROM (SELECT * FROM wx_station ORDER BY timestamp DESC LIMIT $limit) sub ORDER BY timestamp ASC";
$result = $conn->query($sql);

$timestamps = [];
$temperatures = [];
$pressures = [];
$humidities = [];
$lights = [];
$co2s = [];
$tvocs = [];

if ($result->num_rows > 0) {
    while ($row = $result->fetch_assoc()) {
        $timestamps[] = $row['timestamp'];
        $temperatures[] = $row['temperature'];
        $pressures[] = $row['pressure'];
        $humidities[] = $row['humidity'];
        $lights[] = $row['light'];
        $co2s[] = $row['co2_ppm'];
        $tvocs[] = $row['tvoc_ppb'];
    }
}

// Close the connection to the database
$conn->close();

// Create a JSON response
echo json_encode([
    'timestamps' => $timestamps,
    'temperatures' => $temperatures,
    'pressures' => $pressures,
    'humidities' => $humidities,
    'lights' => $lights,
    'co2s' => $co2s,
    'tvocs' => $tvocs
]);
?>
