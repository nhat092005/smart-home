const API_BASE = '';
const FETCH_TIMEOUT = 10000;
let scanning = false;

function showStatus(message, type) {
    const statusEl = document.getElementById('status-message');
    const textEl = document.getElementById('status-text');
    textEl.textContent = message;
    statusEl.className = 'status-message show ' + type;

    setTimeout(function () {
        statusEl.classList.remove('show');
    }, 4000);
}

function fetchWithTimeout(url, options, timeout) {
    return Promise.race([
        fetch(url, options),
        new Promise(function (_, reject) {
            setTimeout(function () {
                reject(new Error('Request timeout'));
            }, timeout);
        })
    ]);
}

function getRSSIClass(rssi) {
    if (rssi >= -60) return 'strong';
    if (rssi >= -75) return 'medium';
    return 'weak';
}

function getRSSIText(rssi) {
    if (rssi >= -60) return 'Excellent';
    if (rssi >= -75) return 'Good';
    return 'Weak';
}

async function scanNetworks() {
    if (scanning) return;

    scanning = true;
    const scanBtn = document.getElementById('scan-btn');
    scanBtn.disabled = true;
    scanBtn.textContent = 'Scanning...';

    showStatus('Scanning WiFi networks...', 'info');

    try {
        const response = await fetchWithTimeout(API_BASE + '/scan', {}, FETCH_TIMEOUT);
        if (!response.ok) {
            throw new Error('Scan failed');
        }

        const networks = await response.json();
        displayNetworks(networks);
        showStatus('Found ' + networks.length + ' networks', 'success');
    } catch (error) {
        console.error('Scan error:', error);
        const errorMsg = error.message === 'Request timeout' ?
            'Scan timeout. Try again.' : 'Scan failed. Try again.';
        showStatus(errorMsg, 'error');
        document.getElementById('network-list').innerHTML =
            '<div class="loading-text">' + errorMsg + '</div>';
    } finally {
        scanning = false;
        scanBtn.disabled = false;
        scanBtn.textContent = 'Scan';
    }
}

function displayNetworks(networks) {
    const list = document.getElementById('network-list');

    if (networks.length === 0) {
        list.innerHTML = '<div class="loading-text">No networks found</div>';
        return;
    }

    networks.sort(function (a, b) {
        return b.rssi - a.rssi;
    });

    list.innerHTML = networks.map(function (net) {
        const signalClass = getRSSIClass(net.rssi);
        const signalText = getRSSIText(net.rssi);
        const security = net.auth === 0 ? 'Open' : 'Secured';

        return '<div class="network-item" onclick="selectNetwork(\'' +
            net.ssid.replace(/'/g, "\\'") + '\', ' + net.auth + ')">' +
            '<div class="network-name">' + net.ssid + ' (' + security + ')</div>' +
            '<div class="network-signal signal-' + signalClass + '">' +
            signalText + ' (' + net.rssi + ' dBm)</div>' +
            '</div>';
    }).join('');
}

function selectNetwork(ssid, authMode) {
    document.getElementById('ssid').value = ssid;

    const passwordField = document.getElementById('password');
    if (authMode === 0) {
        passwordField.value = '';
        passwordField.placeholder = 'Open network - no password required';
        passwordField.disabled = true;
        showStatus('Selected open network', 'info');
    } else {
        passwordField.placeholder = 'Enter password';
        passwordField.disabled = false;
        passwordField.focus();
        showStatus('Network selected', 'info');
    }

    const items = document.querySelectorAll('.network-item');
    for (let i = 0; i < items.length; i++) {
        items[i].classList.remove('selected');
    }
    event.target.closest('.network-item').classList.add('selected');
}

function togglePassword() {
    const passwordField = document.getElementById('password');
    const toggleBtn = document.getElementById('toggle-btn');

    if (passwordField.type === 'password') {
        passwordField.type = 'text';
        toggleBtn.textContent = 'Hide';
    } else {
        passwordField.type = 'password';
        toggleBtn.textContent = 'Show';
    }
}

async function submitForm(event) {
    event.preventDefault();

    const ssid = document.getElementById('ssid').value.trim();
    const password = document.getElementById('password').value;

    if (!ssid) {
        showStatus('Please select or enter a network', 'error');
        return;
    }


    showStatus('Connecting to network...', 'info');

    try {
        const response = await fetchWithTimeout(API_BASE + '/connect', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify({ ssid: ssid, password: password })
        }, FETCH_TIMEOUT);

        if (!response.ok) {
            throw new Error('Connection failed');
        }

        showStatus('Connected successfully!', 'success');
        setTimeout(function () {
            showStatus('Device restarting...', 'info');
        }, 2000);

    } catch (error) {
        console.error('Connect error:', error);
        let errorMsg = 'Connection failed';
        if (error.message === 'Request timeout') {
            errorMsg = 'Connection timeout. Check network.';
        } else if (error.message.includes('credentials')) {
            errorMsg = 'Invalid credentials. Try again.';
        }
        showStatus(errorMsg, 'error');
    }
}