console.log("yo yo ma")

//for example 1
let toggleButton = document.querySelector("#lightToggle");
getButtonState();//immediately gets the current state of the led update the dom

function getButtonState(){
  fetch('/led')
    .then(r => r.json())
    .then(data => {
      toggleButton.textContent = data.lightState ? 'ON' : 'OFF'; //ternery operator! if statement in one line. if state is true, text is on, else its off
    });
}

toggleButton.addEventListener('click', () => {
  fetch('/led', { method: 'POST' }) //changes the state!
    .then(r => r.json())
    .then(data => { //same logic as above, since the server is returning the led state this will update the button to match
      toggleButton.textContent = data.lightState ? 'ON' : 'OFF';
    });
});


//for example 2
//every 3 seconds, fetch the data from the server and update the log
let logContainer = document.querySelector("#log");

//function for fetching data 
function fetchReadings() {
  fetch('/data') //hit this endpoint
    .then(r => r.json()) 
    .then(readings => { 
      // Show only last 10 readings as formatted JSON
      const recent = readings.slice(-10).reverse(); //readings an array, this just pulls the last 10
      logContainer.textContent = JSON.stringify(recent, null, 2); //change it to a string for display
    })
    .catch(error => {
      logElement.textContent = `Error: ${error.message}`; 
    });
}

// Fetch immediately on page load
fetchReadings();

// Then poll every 3 seconds
setInterval(fetchReadings, 3000);



