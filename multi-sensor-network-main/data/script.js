/// Data arrays to store the history for the graphs
const temperatureData = [];
const co2Data = [];
const labels = [];

// Define the threshold for the CO₂ alert (in ppm)
const co2AlertThreshold = 800;

// Get the canvas elements from the HTML
const tempCtx = document.getElementById('temperature-chart').getContext('2d');
const co2Ctx = document.getElementById('co2-chart').getContext('2d');
const batteryAlert = document.getElementById('low-battery-alert');

// NEW: Get modal and room control elements
const addDeviceModal = document.getElementById('add-device-modal');
const addDeviceBtn = document.getElementById('add-device-btn');
const closeModalBtn = document.querySelector('.close-button');
const saveDeviceBtn = document.getElementById('save-device-btn');
const deviceCodeInput = document.getElementById('device-code-input');
const roomNameInput = document.getElementById('room-name-input');
const modalMessage = document.getElementById('modal-message');
const buildingGrid = document.getElementById('buildingGrid');
const levelSwitcher = document.getElementById('levelSwitcher');
const statusSummary = document.getElementById('statusSummary');
const relativeDistances = document.getElementById('relativeDistances');
let currentSensors = [];
let selectedSensorName = null;
// END NEW

// Initialize the Temperature graph
const tempChart = new Chart(tempCtx, {
    type: 'line',
    data: {
        labels: labels,
        datasets: [{
            label: 'Temperature (°C)',
            data: temperatureData,
            borderColor: 'rgb(75, 192, 192)',
            tension: 0.1
        }]
    },
    options: {
        scales: {
            y: {
                beginAtZero: false,
                title: {
                    display: true,
                    text: 'Temperature (°C)'
                }
            },
            x: {
                title: {
                    display: true,
                    text: 'Time'
                }
            }
        }
    }
});

// Initialize the CO₂ graph
const co2Chart = new Chart(co2Ctx, {
    type: 'line',
    data: {
        labels: labels,
        datasets: [{
            label: 'CO₂ Level (ppm)',
            data: co2Data,
            borderColor: 'rgb(255, 99, 132)',
            tension: 0.1
        }]
    },
    options: {
        scales: {
            y: {
                beginAtZero: false,
                title: {
                    display: true,
                    text: 'CO₂ Level (ppm)'
                }
            }
        }
    }
});

// A new function to simulate a low battery status
function isBatteryLow() {
    // This will return true ~10% of the time for demonstration
    return Math.random() < 0.1; 
}

// Function to fetch data from the Open-Meteo API
async function getSensorData() {
    // In a real application, this API call would use the currently selected device/room ID
    // For this example, we keep the original fixed call.
    const apiUrl = 'https://api.open-meteo.com/v1/forecast?latitude=51.4408&longitude=5.4779&current=temperature_2m,shortwave_radiation';
    try {
        const response = await fetch(apiUrl);
        if (!response.ok) {
            throw new Error(`HTTP error! status: ${response.status}`);
        }
        const data = await response.json();
        const temperature = data.current.temperature_2m.toFixed(1);
        const solarRadiation = data.current.shortwave_radiation.toFixed(0);
        
        // This is still a simulated value
        const co2 = (Math.random() * 800 + 400).toFixed(0); 
        
        return {
            temperature: `${temperature} °C`,
            co2: `${co2} ppm`,
            solarRadiation: `${solarRadiation} W/m²`,
            rawTemp: parseFloat(temperature),
            rawCo2: parseInt(co2)
        };
    } catch (error) {
        console.error("Could not fetch data:", error);
        return {
            temperature: 'N/A',
            co2: 'N/A',
            solarRadiation: 'N/A',
            rawTemp: null,
            rawCo2: null
        };
    }
}

// Function to update the HTML page and both graphs
async function updateDashboard() {
    
    const data = await getSensorData();

    // Get the CO₂ card element to add/remove the alert class
    const co2Card = document.querySelector('.sensor-card:nth-child(2)');

    // Update the HTML text displays
    document.getElementById('temperature-value').textContent = data.temperature;
    document.getElementById('co2-value').textContent = data.co2;
    document.getElementById('solar-radiation-value').textContent = data.solarRadiation;

    // Check if the CO₂ level exceeds the threshold and apply the alert class
    if (data.rawCo2 > co2AlertThreshold) {
        co2Card.classList.add('alert');
    } else {
        co2Card.classList.remove('alert');
    }

    // Check for low battery and show the alert if necessary
    if (isBatteryLow()) {
        batteryAlert.classList.remove('hidden');
    } else {
        batteryAlert.classList.add('hidden');
    }

    // --- Graph Update Logic ---
    if (data.rawTemp !== null && data.rawCo2 !== null) {
        temperatureData.push(data.rawTemp);
        co2Data.push(data.rawCo2);

        const now = new Date();
        labels.push(`${now.getHours()}:${now.getMinutes()}:${now.getSeconds()}`);

        const maxPoints = 20;
        if (labels.length > maxPoints) {
            temperatureData.shift();
            co2Data.shift();
            labels.shift();
        }

        tempChart.update();
        co2Chart.update();
    }
}

// NEW: Functions for the Add Device Modal
function openModal() {
    addDeviceModal.classList.remove('hidden');
    modalMessage.classList.add('hidden'); // Clear any previous message
    deviceCodeInput.value = ''; // Clear previous input
    roomNameInput.value = '';    // Clear previous input
}

function closeModal() {
    addDeviceModal.classList.add('hidden');
}

function handleSaveDevice() {
    const code = deviceCodeInput.value.trim();
    const roomName = roomNameInput.value.trim();

    if (code && roomName) {
        // --- In a real application, you would send this data to a server ---
        console.log(`Saving new device: Code=${code}, Room=${roomName}`);
        modalMessage.textContent = `✅ Success! Device ${code} added for ${roomName}.`;
        modalMessage.classList.remove('hidden');
        
        // Close the modal after a short delay
        setTimeout(closeModal, 2000); 

    } else {
        modalMessage.textContent = '❌ Please enter both a device code and a room name.';
        modalMessage.classList.remove('hidden');
    }
}

// Event Listeners for the modal
addDeviceBtn.addEventListener('click', openModal);
closeModalBtn.addEventListener('click', closeModal);
saveDeviceBtn.addEventListener('click', handleSaveDevice);

// Close modal if user clicks outside of it
window.addEventListener('click', (event) => {
    if (event.target === addDeviceModal) {
        closeModal();
    }
});
// END NEW

// Facility map data and interactions
const buildingLevels = [
    {
        id: 'level-1',
        label: 'Level 1',
        sensors: [
            { name: 'ESP-North', status: 'active', distance: '12.4 m', top: 20, left: 25 },
            { name: 'ESP-East', status: 'active', distance: '9.8 m', top: 35, left: 70 },
            { name: 'ESP-West', status: 'inactive', distance: '15.1 m', top: 52, left: 18 },
            { name: 'ESP-Lab', status: 'active', distance: '7.2 m', top: 55, left: 58 },
            { name: 'ESP-Office', status: 'inactive', distance: '5.9 m', top: 72, left: 40 },
            { name: 'ESP-Storage', status: 'active', distance: '18.6 m', top: 78, left: 78 }
        ]
    },
    {
        id: 'level-2',
        label: 'Level 2',
        sensors: [
            { name: 'ESP-North 2', status: 'active', distance: '11.1 m', top: 18, left: 30 },
            { name: 'ESP-East 2', status: 'inactive', distance: '10.4 m', top: 33, left: 63 },
            { name: 'ESP-West 2', status: 'active', distance: '8.6 m', top: 48, left: 20 },
            { name: 'ESP-Lab 2', status: 'active', distance: '6.4 m', top: 50, left: 57 },
            { name: 'ESP-Office 2', status: 'inactive', distance: '6.8 m', top: 65, left: 45 },
            { name: 'ESP-Storage 2', status: 'active', distance: '14.2 m', top: 75, left: 75 }
        ]
    },
    {
        id: 'level-3',
        label: 'Level 3',
        sensors: [
            { name: 'ESP Research', status: 'inactive', distance: '12.9 m', top: 18, left: 52 },
            { name: 'ESP South Hall', status: 'active', distance: '20.4 m', top: 32, left: 75 },
            { name: 'ESP Admin', status: 'active', distance: '9.3 m', top: 38, left: 33 },
            { name: 'ESP Data Center', status: 'inactive', distance: '17.2 m', top: 58, left: 65 },
            { name: 'ESP Breakout', status: 'active', distance: '13.7 m', top: 68, left: 35 },
            { name: 'ESP Atrium', status: 'active', distance: '8.5 m', top: 78, left: 70 }
        ]
    }
];

function renderLevelButtons() {
    levelSwitcher.innerHTML = '';
    buildingLevels.forEach((level, index) => {
        const button = document.createElement('button');
        button.type = 'button';
        button.className = `level-button ${index === 0 ? 'active' : ''}`;
        button.textContent = level.label;
        button.dataset.levelId = level.id;
        button.addEventListener('click', () => setActiveLevel(level.id));
        levelSwitcher.appendChild(button);
    });
}

function setActiveLevel(levelId) {
    document.querySelectorAll('.level-button').forEach((button) => {
        button.classList.toggle('active', button.dataset.levelId === levelId);
    });

    const level = buildingLevels.find((lvl) => lvl.id === levelId);
    if (!level) return;
    renderSensors(level.sensors);
    renderSummary(level.sensors);
}

function renderSensors(sensors) {
    currentSensors = sensors;
    selectedSensorName = null;
    buildingGrid.innerHTML = '';
    sensors.forEach((sensor) => {
        const point = document.createElement('div');
        point.className = 'esp-point';
        point.style.top = `${sensor.top}%`;
        point.style.left = `${sensor.left}%`;
        point.dataset.status = sensor.status === 'active' ? 'active' : 'inactive';
        point.dataset.sensorName = sensor.name;

        const tooltip = document.createElement('span');
        tooltip.className = 'tooltip';
        tooltip.textContent = `${sensor.name} • ${sensor.status === 'active' ? 'Online' : 'Offline'}`;
        point.appendChild(tooltip);

        const distance = document.createElement('span');
        distance.className = 'distance-label';
        point.appendChild(distance);

        point.addEventListener('click', () => handleSensorClick(sensor.name));

        buildingGrid.appendChild(point);
    });
    updateDistanceLabels();
    updateRelativePanel();
}

function renderSummary(sensors) {
    const activeCount = sensors.filter((sensor) => sensor.status === 'active').length;
    const inactiveCount = sensors.length - activeCount;

    statusSummary.innerHTML = `
        <div class="summary-card">
            <span>Active</span>
            <strong>${activeCount}</strong>
        </div>
        <div class="summary-card">
            <span>Offline</span>
            <strong>${inactiveCount}</strong>
        </div>
        <div class="summary-card">
            <span>Total Distance</span>
            <strong>${sumDistances(sensors)}</strong>
        </div>
    `;
}

function sumDistances(sensors) {
    const total = sensors.reduce((sum, sensor) => {
        const value = parseFloat(sensor.distance);
        return Number.isFinite(value) ? sum + value : sum;
    }, 0);
    return `${total.toFixed(1)} m`;
}

function handleSensorClick(sensorName) {
    selectedSensorName = sensorName;
    document.querySelectorAll('.esp-point').forEach((point) => {
        point.classList.toggle('selected', point.dataset.sensorName === sensorName);
    });
    updateDistanceLabels();
    updateRelativePanel();
}

function updateDistanceLabels() {
    const selected = currentSensors.find((sensor) => sensor.name === selectedSensorName);
    document.querySelectorAll('.distance-label').forEach((label) => {
        label.classList.remove('visible');
        label.textContent = '';
    });
    if (!selected) return;

    currentSensors.forEach((sensor) => {
        const point = buildingGrid.querySelector(`.esp-point[data-sensor-name="${sensor.name}"]`);
        if (!point) return;
        const label = point.querySelector('.distance-label');
        if (!label) return;

        if (sensor.name === selected.name) {
            label.textContent = '0.0 m';
        } else {
            const relative = calculateRelativeDistance(selected, sensor);
            label.textContent = formatMeters(relative);
        }
        label.classList.add('visible');
    });
}

function calculateRelativeDistance(sensorA, sensorB) {
    const deltaX = sensorA.left - sensorB.left;
    const deltaY = sensorA.top - sensorB.top;
    const planarDistance = Math.hypot(deltaX, deltaY);
    return planarDistance * 0.6;
}

function formatMeters(value) {
    return `${value.toFixed(1)} m`;
}

function updateRelativePanel() {
    relativeDistances.innerHTML = '';
    const selected = currentSensors.find((sensor) => sensor.name === selectedSensorName);

    if (!selected) {
        relativeDistances.innerHTML = '<div class="relative-placeholder">Select a sensor on the map to compare.</div>';
        return;
    }

    currentSensors
        .filter((sensor) => sensor.name !== selected.name)
        .map((sensor) => {
            const rawDistance = calculateRelativeDistance(selected, sensor);
            return {
                name: sensor.name,
                value: rawDistance,
                label: formatMeters(rawDistance)
            };
        })
        .sort((a, b) => a.value - b.value)
        .forEach((entry) => {
            const row = document.createElement('div');
            row.className = 'relative-item';
            row.innerHTML = `<span>${entry.name}</span><strong>${entry.label}</strong>`;
            relativeDistances.appendChild(row);
        });
}

if (buildingGrid && levelSwitcher && statusSummary && relativeDistances) {
    renderLevelButtons();
    setActiveLevel(buildingLevels[0].id);
}

// Update the dashboard every 3 seconds
setInterval(updateDashboard, 3000);

// Run the function once when the page first loads
updateDashboard();