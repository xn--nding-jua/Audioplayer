function setGate(gate) {
	// send data to microcontroller
	fetch('/cmd.php?mixer:gate' + gate + '=' + parseFloat(document.getElementById('slider_gate' + gate + '_thresh').value).toFixed(1) + ',' + document.getElementById('slider_gate' + gate + '_range').value + ',' + document.getElementById('slider_gate' + gate + '_att').value + ',' + document.getElementById('slider_gate' + gate + '_hold').value + ',' + document.getElementById('slider_gate' + gate + '_rel').value);
}

function setComp(comp) {
	// send data to microcontroller
	var ratio = document.getElementById('select_comp' + comp + '_ratio').selectedIndex;
	if (ratio > 0) {
		ratio = Math.pow(2, ratio-1);
	}
	fetch('/cmd.php?mixer:comp' + comp + '=' + parseFloat(document.getElementById('slider_comp' + comp + '_thresh').value).toFixed(1) + ',' + ratio + ',' + document.getElementById('slider_comp' + comp + '_makeup').value + ',' + document.getElementById('slider_comp' + comp + '_att').value + ',' + document.getElementById('slider_comp' + comp + '_hold').value + ',' + document.getElementById('slider_comp' + comp + '_rel').value);
}