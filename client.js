$(function () {
	var getDec = function(num) {
		if(Math.floor(num) === num) return 0;
		return num.toString().split(".")[1].length || 0;
	}
	var convertGpsData = function(lat, long) {
		try {
			console.log('Latitude = ' + lat);
			console.log('Longitude = ' + long);
			//lat = lat <= 0 ? Math.abs(lat) : lat;
			//long = long <= 0 ? Math.abs(long) : long;
			var lat_neg = (lat < 0);
			var long_neg = (long < 0);
			var lat_d = lat_neg ? lat.toString().substring(0, 3) : lat.toString().substring(0, 2);
			console.log(`Lat_D = ${lat_d}`);
			var lat_m = lat_neg ? lat.toString().substring(3) : lat.toString().substring(2);
			var lat_dd = parseFloat(lat_d) + parseFloat(lat_m) / 60.0;
			var long_d = long_neg ? long.toString().substring(0, 3) : long.toString().substring(0, 2);
			var long_m = long_neg ? long.toString().substring(3) : long.toString().substring(2);
//			var long_dd = parseFloat(long_d) + parseFloat(long_m) / 60.0;

			if(long_d > 0)
				long_dd = parseFloat(long_d) + parseFloat(long_m) / 60.0;
			else
				long_dd = parseFloat(long_d) - parseFloat(long_m) / 60.0;
				
			if(lat_d > 0)
				lat_dd = parseFloat(lat_d) + parseFloat(lat_m) / 60.0;
			else
				lat_dd = parseFloat(lat_d) - parseFloat(lat_m) / 60.0;
			console.log(lat_dd);
			console.log(long_dd);
		}
		catch(err) {
			console.log('Error converting lat & long');
			console.log(err);
		}
		return {lat_dd, long_dd};
	}
	// if user is running mozilla then use it's built-in WebSocket
	window.WebSocket = window.WebSocket || window.MozWebSocket;
	var content = $('#content');
	var connection = new WebSocket('ws://smartmirror-bryton.local:8080', "nav");
	if (!window.WebSocket) {
		content.html($('<p>', { text:'Sorry, but your browser doesn\'t support WebSocket.'}));
		input.hide();
		$('span').hide();
		return;
	}

	connection.onopen = function () {
	// connection is opened and ready to use
		console.log('Connection opened.');
		$('#isCon').show();
	//	connection.send('Hello');
	};

	connection.onerror = function (error) {
	// an error occurred when sending/receiving data
		console.log(error);
	};
	connection.onmessage = function (message) 
	{
		try
		{
			if(message instanceof Blob) {
				console.log("Received binary frame.");
			}
			else {
				var json = JSON.parse(message.data);
				var d = json.sensor_data;
				$('li#cel')[0].innerText= d.BMP.temp_c + '\xB0 C';
				$('li#fahr')[0].innerText= d.BMP.temp_f + '\xB0 F';
				$('li#pressure')[0].innerText = d.BMP.pressure + ' hPa';
				$('li#heading')[0].innerText = d.LSM.heading + '\xB0';
				gauge.update({value: d.LSM.heading});
				$('li#pitch')[0].innerText = d.LSM.pitch + '\xB0';
				$('li#roll')[0].innerText = d.LSM.roll + '\xB0';
				$('li#x_mag')[0].innerText = d.LSM.x_mag;
				$('li#y_mag')[0].innerText = d.LSM.y_mag;
				$('li#z_mag')[0].innerText = d.LSM.z_mag;
				$('li#x_acc')[0].innerText = d.LSM.x_acc;
				$('li#y_acc')[0].innerText = d.LSM.y_acc;
				$('li#z_acc')[0].innerText = d.LSM.z_acc;
				var l = convertGpsData(d.GPS.lat, d.GPS.long);
				$('li#lat')[0].innerText = l.lat_dd + '\xB0 ' + d.GPS.ew;
				$('li#long')[0].innerText = l.long_dd + '\xB0 ' + d.GPS.ns;
				console.log(`Lat = ${l.lat_dd}, Long = ${l.long_dd}`);
				if(!mapHasBeenSet) {
					var s = {lat: l.lat_dd, lng: l.long_dd };
					var map = new google.maps.Map(document.getElementById('mapid'), {zoom: 4, center: s });
					var marker = new google.maps.Marker({position: s, map: map});
					mapHasBeenSet = true;
					setInterval(function() { 
						mapHasBeenSet = false;
					}, 3600000);
				}
			}
		} 
		catch (e)
		{
			console.log('This doesn\'t look like a valid JSON: ', message.data);
			return;
		}
	};
});
