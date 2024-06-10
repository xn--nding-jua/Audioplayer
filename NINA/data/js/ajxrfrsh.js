// a enum-like array for the filters
//const EQ_Types = ['Allpass', 'Peak', 'LowShelf', 'HighShelf', 'Bandpass', 'Notch', 'LowPass', 'HighPass'];

var url ='http://' + location.host + '/json';
function renew(){
 document.getElementById('version_mainctrl').style.color = "rgb(0, 255, 0)";

 fetch(url)
 .then(response => {return response.json();})
 .then(jsonOutput => {

	// set time
	document.getElementById('sec').innerHTML = Math.floor(jsonOutput['system_uptime'] % 60) + "s"; 
	if (jsonOutput['system_uptime'] >= 60) { document.getElementById('min').innerHTML = Math.floor(jsonOutput['system_uptime'] / 60 % 60) + "min";}
	if (jsonOutput['system_uptime'] >= 3600) {document.getElementById('hour').innerHTML = Math.floor(jsonOutput['system_uptime'] / 3600 % 24) + "h";}
	if (jsonOutput['system_uptime'] >= 86400) {document.getElementById('day').innerHTML = Math.floor(jsonOutput['system_uptime'] / 86400 % 7) + "days";}
	if (jsonOutput['system_uptime'] >= 604800) {document.getElementById('week').innerHTML = Math.floor(jsonOutput['system_uptime'] / 604800 % 52) + "weeks";}

	// set progressbar-position
	var bar = document.querySelector(".progress-bar");
	bar.style.width = jsonOutput['player_progress'];
	bar.innerText = jsonOutput['player_progress'];
	// change progressbar-color
	if (jsonOutput['player_isplaying']) {
		document.getElementById('player_progressbar').setAttribute('class', 'progress-bar bg-success progress-bar-striped progress-bar-animated');
	}else{
		document.getElementById('player_progressbar').setAttribute('class', 'progress-bar bg-danger progress-bar-striped');
	}
	 
	// set ADC-Gains
	for (let adc = 1; adc <= 1; adc++) {
		document.getElementById('select_adc' + adc + '_gain').selectedIndex = Math.round(jsonOutput['adc' + adc + '_gain'] / 6); // convert dB (6dB/bit) to selectedIndex
	}
	// set PEQ-types
	for (let peq = 1; peq <= 5; peq++) {
		//document.getElementById('select_peq' + peq + '_type').value = EQ_Types[jsonOutput['peq' + peq + '_type']];
		document.getElementById('select_peq' + peq + '_type').selectedIndex = jsonOutput['peq' + peq + '_type']; // take number of type as selectedIndex
	}
	// set Compressor-ratio
	for (let comp = 1; comp <= 2; comp++) {
		var ratio = jsonOutput['comp' + comp + '_ratio'];
		if (ratio > 0) {
			ratio = Math.log2(ratio) + 1; // convert dB to selectedIndex
		}
		document.getElementById('select_comp' + comp + '_ratio').selectedIndex = ratio;
	}
	// set VU-meters
	document.getElementById('vumeter_left').setAttribute('data-val', jsonOutput['vumeter_left']);
	document.getElementById('vumeter_right').setAttribute('data-val', jsonOutput['vumeter_right']);
	document.getElementById('vumeter_sub').setAttribute('data-val', jsonOutput['vumeter_sub']);

	// append volume-information to the time-series-chart
	if (timeseries?.["timeseries_left"] !== undefined) timeseries["timeseries_left"].append(new Date().getTime(), parseFloat(jsonOutput['vumeter_left'])/2.55);
	if (timeseries?.["timeseries_right"] !== undefined) timeseries["timeseries_right"].append(new Date().getTime(), parseFloat(jsonOutput['vumeter_right'])/2.55);
	if (timeseries?.["timeseries_sub"] !== undefined) timeseries["timeseries_sub"].append(new Date().getTime(), parseFloat(jsonOutput['vumeter_sub'])/2.55);

	// set power
	var vsquaremain = jsonOutput['power_main']*6; // convert value for 6ohm to generic v^2 value
	var vsquaresub = jsonOutput['power_sub']*6; // convert value for 6ohm to generic v^2 value
	document.getElementById('power_main_overview').innerHTML = "Peak: " + Math.round(vsquaremain/4) + "W / " + Math.round(vsquaremain/6) + "W / " + Math.round(vsquaremain/8) + "W<br>" + "RMS: " + Math.round((vsquaremain/Math.sqrt(2))/4) + "W / " + Math.round((vsquaremain/Math.sqrt(2))/6) + "W / " + Math.round((vsquaremain/Math.sqrt(2))/8) + "W";
	document.getElementById('power_sub_overview').innerHTML = "Peak: " + Math.round(vsquaresub/4) + "W / " + Math.round(vsquaresub/6) + "W / " + Math.round(vsquaresub/8) + "W<br>" + "RMS: " + Math.round((vsquaresub/Math.sqrt(2))/4) + "W / " + Math.round((vsquaresub/Math.sqrt(2))/6) + "W / " + Math.round((vsquaresub/Math.sqrt(2))/8) + "W";

	
	// set clip- and comp-badges
	if (jsonOutput['comp1_active'] == 1) {
		document.getElementById('comp_main_left').setAttribute('class', 'badge bg-warning');
		document.getElementById('comp_main_right').setAttribute('class', 'badge bg-warning');
	}else{
		document.getElementById('comp_main_left').setAttribute('class', 'badge bg-secondary text-dark');
		document.getElementById('comp_main_right').setAttribute('class', 'badge bg-secondary text-dark');
	}
	if (jsonOutput['comp2_active'] == 1) {
		document.getElementById('comp_main_sub').setAttribute('class', 'badge bg-warning');
	}else{
		document.getElementById('comp_main_sub').setAttribute('class', 'badge bg-secondary text-dark');
	}
	if (jsonOutput['clip_left'] == 1) {
		document.getElementById('clip_main_left').setAttribute('class', 'badge bg-danger');
	}else{
		document.getElementById('clip_main_left').setAttribute('class', 'badge bg-secondary text-dark');
	}
	if (jsonOutput['clip_right'] == 1) {
		document.getElementById('clip_main_right').setAttribute('class', 'badge bg-danger');
	}else{
		document.getElementById('clip_main_right').setAttribute('class', 'badge bg-secondary text-dark');
	}
	if (jsonOutput['clip_sub'] == 1) {
		document.getElementById('clip_main_sub').setAttribute('class', 'badge bg-danger');
	}else{
		document.getElementById('clip_main_sub').setAttribute('class', 'badge bg-secondary text-dark');
	}
	if (jsonOutput['gate1_closed'] == 1) {
		document.getElementById('gate1_state').setAttribute('class', 'badge bg-danger');
		document.getElementById('gate1_state').innerHTML = 'Gate closed';
	}else{
		document.getElementById('gate1_state').setAttribute('class', 'badge bg-success');
		document.getElementById('gate1_state').innerHTML = 'Gate open';
	}

	// try to set sliders based on the element-name
	for (var i in jsonOutput)
    {
		// add data to page-elements
		if (document.getElementById(i)) {
			document.getElementById(i).innerHTML = jsonOutput[i];

			try {
				if (!document.getElementById('slider_' + i).matches(':hover')) {
					// update sliders only, when mouse is not hovering
					if (i.includes("_fc")) {
						// frequency-slider -> convert back to linear
						document.getElementById('slider_' + i).value = freq2slider(jsonOutput[i]);
					}else{
						// standard-slider
						document.getElementById('slider_' + i).value = jsonOutput[i];
					}
				}
			}catch{
				// we will end here if a slider is not available for the specific value. Don't care about this.
			}
		};
	}

	// update EQ-diagram
	updateEQ();

	// everything is fine. Turn text to white
	document.getElementById('version_mainctrl').style.color = "rgb(255, 255, 255)";
 })
 .catch(function() {
  document.getElementById('version_mainctrl').style.color = "rgb(255, 0, 0)";
 });
}
document.addEventListener('DOMContentLoaded', renew, setInterval(renew, 100)); // 100ms as updaterate between NINA and HTTP-Client (e.g. Computer or Mobilephone)