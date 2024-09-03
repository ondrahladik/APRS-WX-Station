<!DOCTYPE html>
<html lang="en">
<head> 
    <meta charset="UTF-8">  
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <link rel="apple-touch-icon" sizes="180x180" href="img/apple-touch-icon.png">
    <link rel="icon" type="image/png" sizes="32x32" href="img/favicon-32x32.png">
    <link rel="icon" type="image/png" sizes="16x16" href="img/favicon-16x16.png">
    <link rel="manifest" href="img/site.webmanifest">
    <meta name="msapplication-TileColor" content="#da532c">
    <meta name="theme-color" content="#ffffff">
    <link rel="stylesheet" type="text/css" href="css/master.css">
    <link rel="stylesheet" type="text/css" href="css/graf.css">
    <title>GRAF</title>
</head>
<body>

    <nav>
        <img class="left" src="img/apple-touch-icon.png" alt="wx">
        <img class="right" src="img/apple-touch-icon.png" alt="wx">
        <h1>APRS WX STATION</h1>
        <a href="/">DASH</a>
        <a class="active" href="graf.php?data=temp&time=1h">GRAF</a>
        <a href="info.html">INFO</a>
    </nav>

    <footer>
        2024 &copy; <a href="https://www.ok1kky.cz" target="_BLANK">OK1KKY</a>
    </footer>

    <div class='chart-container'>
        <div class='btn-group'>
            <button id='btnTemp' class='active' onclick='showData("temp")'>Temp</button>
            <button id='btnHum' onclick='showData("humi")'>Humi</button>
            <button id='btnPress' onclick='showData("pres")'>Pres</button>
            <button id='btnLight' onclick='showData("light")'>Ligh</button>
            <button id='btneco2' onclick='showData("eco2")'>eCO2</button>
            <button id='btntovc' onclick='showData("tvoc")'>TVOC</button>
        </div>
        <div class='time-group'>
            <button id='btn1h' class='active' onclick='changeTimeRange("1h")'>1 h</button>
            <button id='btn6h' onclick='changeTimeRange("6h")'>6 h</button>
            <button id='btn12h' onclick='changeTimeRange("12h")'>12 h</button>
            <button id='btn24h' onclick='changeTimeRange("24h")'>24 h</button>
        </div>
        <div class='chart-wrapper'>
            <canvas id='weatherChart'></canvas>
        </div>
    </div>

<script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
<script src="https://cdn.jsdelivr.net/npm/jquery@3.6.0/dist/jquery.min.js"></script>
<script src="js/graf.js"></script>


</body>
</html>
