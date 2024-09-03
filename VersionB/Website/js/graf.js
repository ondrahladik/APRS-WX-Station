var weatherChart;
    var timestamps = [];
    var temperatures = [];
    var pressures = [];
    var humidities = [];
    var lights = [];
    var eco2s = []; 
    var tvocs = [];

    function initialize() {
        var currentUrl = new URL(window.location.href);
        var hasDataParam = currentUrl.searchParams.has('data');
        var hasTimeParam = currentUrl.searchParams.has('time');

        if (!hasDataParam || !hasTimeParam) {
            currentUrl.searchParams.set('data', 'temp');
            currentUrl.searchParams.set('time', '1h');
            window.location.href = currentUrl.toString();
            return;
        }

        var initialData = currentUrl.searchParams.get('data');
        var initialTime = currentUrl.searchParams.get('time');

        document.getElementById('btn1h').classList.toggle('active', initialTime === '1h');
        document.getElementById('btn6h').classList.toggle('active', initialTime === '6h');
        document.getElementById('btn12h').classList.toggle('active', initialTime === '12h');
        document.getElementById('btn24h').classList.toggle('active', initialTime === '24h');

        showData(initialData);

        setInterval(function() {
            var currentUrl = new URL(window.location.href);
            var time = currentUrl.searchParams.get('time');
            var data = currentUrl.searchParams.get('data');
            fetchData(time, data);
        }, 30000); // Update every 30 seconds
        }

        function fetchData(time, data) {
            var url = 'fetch_graf.php?time=' + time + '&data=' + data;
            fetch(url)
                .then(response => response.json())
                .then(json => {
                    timestamps = json.timestamps;
                    temperatures = json.temperatures.map(Number);
                    pressures = json.pressures.map(Number);
                    humidities = json.humidities.map(Number);
                    lights = json.lights.map(Number);
                    eco2s = json.co2s.map(Number);
                    tvocs = json.tvocs.map(Number);

                    updateChart(data);
                })
                .catch(error => console.error('Error fetching data:', error));
        }


        function updateChart(data) {
        var chartData = [];
        var label = '';
        var backgroundColor = '';
        var borderColor = '';
        var minVal, maxVal;

        if (data === 'temp') {
            chartData = temperatures;
            label = 'Temperature';
            backgroundColor = 'rgba(255, 99, 132, 0.2)';
            borderColor = 'rgba(255, 99, 132, 1)';
            minVal = Math.min.apply(null, temperatures) - 0.2;
            maxVal = Math.max.apply(null, temperatures) + 0.2;
        } else if (data === 'humi') {
            chartData = humidities;
            label = 'Humidity';
            backgroundColor = 'rgba(54, 162, 235, 0.2)';
            borderColor = 'rgba(54, 162, 235, 1)';
            minVal = Math.min.apply(null, humidities) - 0.2; 
            maxVal = Math.max.apply(null, humidities) + 0.2;
        } else if (data === 'pres') {
            chartData = pressures;
            label = 'Pressure';
            backgroundColor = 'rgba(75, 192, 192, 0.2)';
            borderColor = 'rgba(75, 192, 192, 1)';
            minVal = Math.min.apply(null, pressures) - 0.2; 
            maxVal = Math.max.apply(null, pressures) + 0.2;
        } else if (data === 'light') {
            chartData = light;
            label = 'Lihts';
            backgroundColor = 'rgba(255, 206, 86, 0.2)';
            borderColor = 'rgba(255, 206, 86, 1)';
            minVal = Math.min.apply(null, lights) - 0.2; 
            maxVal = Math.max.apply(null, lights) + 0.2;
        } else if (data === 'eco2') {
            chartData = eco2s;
            label = 'eCO2';
            backgroundColor = 'rgba(153, 102, 255, 0.2)';
            borderColor = 'rgba(153, 102, 255, 1)';
            minVal = Math.min.apply(null, eco2s) - 0.2; 
            maxVal = Math.max.apply(null, eco2s) + 0.2;
        } else if (data === 'tvoc') {
            chartData = tvocs;
            label = 'TVOC';
            backgroundColor = 'rgba(255, 129, 3, 0.2)';
            borderColor = 'rgba(255, 129, 3, 1)';
            minVal = Math.min.apply(null, tvocs) - 0.2; 
            maxVal = Math.max.apply(null, tvocs) + 0.2;
        }

        if (weatherChart) {
            weatherChart.destroy();
        }

        var ctx = document.getElementById('weatherChart').getContext('2d');
        weatherChart = new Chart(ctx, {
            type: 'line',
            data: {
                labels: timestamps,
                datasets: [{
                    label: label,
                    data: chartData,
                    backgroundColor: backgroundColor,
                    borderColor: borderColor,
                    borderWidth: 1,
                    fill: true,
                }]
            },
            options: {
                animation: false,
                scales: {
                    y: {
                        beginAtZero: false,
                        suggestedMin: minVal, 
                        suggestedMax: maxVal, 
                        ticks: {
                            color: 'rgba(255, 255, 255, 1)',
                            font: {
                                weight: 'bold',
                                size: 12,
                                family: 'Arial',
                            }
                        }
                    },
                    x: {
                        display: false 
                    }
                },
                plugins: {
                    legend: {
                        display: false 
                    }
                }
            }
        });
    }


        function showData(data) {
            var currentUrl = new URL(window.location.href);
            currentUrl.searchParams.set('data', data);
            window.history.replaceState({}, '', currentUrl);

            var time = currentUrl.searchParams.get('time');
            fetchData(time, data);

            document.getElementById('btnTemp').classList.remove('active');
            document.getElementById('btnHum').classList.remove('active');
            document.getElementById('btnPress').classList.remove('active');
            document.getElementById('btnLight').classList.remove('active');
            document.getElementById('btneco2').classList.remove('active');
            document.getElementById('btntovc').classList.remove('active');

            if (data === 'temp') {
                document.getElementById('btnTemp').classList.add('active');
            } else if (data === 'humi') {
                document.getElementById('btnHum').classList.add('active');
            } else if (data === 'pres') {
                document.getElementById('btnPress').classList.add('active');
            } else if (data === 'light') {
                document.getElementById('btnLight').classList.add('active');
            } else if (data === 'eco2') { 
                document.getElementById('btneco2').classList.add('active');
            } else if (data === 'tvoc') { 
                document.getElementById('btntovc').classList.add('active');
            }
        }

        function changeTimeRange(time) {
            var currentUrl = new URL(window.location.href);
            currentUrl.searchParams.set('time', time);
            window.history.replaceState({}, '', currentUrl);

            var data = currentUrl.searchParams.get('data');
            fetchData(time, data);

            document.getElementById('btn1h').classList.remove('active');
            document.getElementById('btn6h').classList.remove('active');
            document.getElementById('btn12h').classList.remove('active');
            document.getElementById('btn24h').classList.remove('active');

            if (time === '1h') {
                document.getElementById('btn1h').classList.add('active');
            } else if (time === '6h') {
                document.getElementById('btn6h').classList.add('active');
            } else if (time === '12h') {
                document.getElementById('btn12h').classList.add('active');
            } else if (time === '24h') {
                document.getElementById('btn24h').classList.add('active');
            }
        }

        initialize();