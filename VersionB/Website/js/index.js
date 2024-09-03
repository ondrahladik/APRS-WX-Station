        function fetchData() {
            $.ajax({
                url: 'fetch_data.php',
                type: 'GET',
                dataType: 'json',
                success: function(data) {
                    if (data) {
                        $('#temperature').text(data.temperature + ' °C');
                        $('#pressure').text(data.pressure + ' hPa');
                        $('#humidity').text(data.humidity + ' %');
                        $('#light').text(data.light + ' W/m2');
                        $('#co2_ppm').text(data.co2_ppm + ' ppm');
                        $('#tvoc_ppm').text(data.tvoc_ppb + ' ppb');
                        $('#timestamp').text(data.timestamp);
                    }
                },
                error: function() {
                    console.error('Failed to load data.');
                }
            });
        }

        $(document).ready(function() {
            fetchData(); 
            setInterval(fetchData, 5000); 
        });