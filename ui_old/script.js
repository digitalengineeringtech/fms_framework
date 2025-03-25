// Configuration
const wsProtocol = window.location.protocol === 'https:' ? 'wss:' : 'ws:';
const wsUrl = `${wsProtocol}//${window.location.hostname}:81`;
const apiBaseUrl = window.location.origin;

// Elements
const connectionStatus = document.getElementById('connection-status');
const deviceName = document.getElementById('device-name');
const ipAddress = document.getElementById('ip-address');
const macAddress = document.getElementById('mac-address');
const currentVersion = document.getElementById('current-version');
const latestVersion = document.getElementById('latest-version');
const uptime = document.getElementById('uptime');
const freeMemory = document.getElementById('free-memory');
const flashSize = document.getElementById('flash-size');
const footerVersion = document.getElementById('footer-version');

const updateStatus = document.getElementById('update-status');
const progressContainer = document.getElementById('progress-container');
const progressBarInner = document.getElementById('progress-bar-inner');
const progressText = document.getElementById('progress-text');
const errorMessage = document.getElementById('error-message');

const checkUpdatesBtn = document.getElementById('check-updates-btn');
const updateBtn = document.getElementById('update-btn');
const restartBtn = document.getElementById('restart-btn');

// WebSocket connection
let socket;
let reconnectAttempts = 0;
let reconnectTimeout;

// Device information
let deviceInfo = {};

// Initialize the application
function init() {
  connectWebSocket();
  fetchDeviceInfo();
  
  // Set up event listeners
  checkUpdatesBtn.addEventListener('click', checkForUpdates);
  updateBtn.addEventListener('click', startUpdate);
  restartBtn.addEventListener('click', restartDevice);
  
  // Refresh device info every 30 seconds
  setInterval(fetchDeviceInfo, 30000);
}

// Connect to WebSocket server
function connectWebSocket() {
  connectionStatus.textContent = 'Connecting...';
  connectionStatus.className = 'connecting';
  
  socket = new WebSocket(wsUrl);
  
  socket.onopen = function() {
    console.log('WebSocket connected');
    connectionStatus.textContent = 'Connected';
    connectionStatus.className = 'connected';
    reconnectAttempts = 0;
  };
  
  socket.onclose = function() {
    console.log('WebSocket disconnected');
    connectionStatus.textContent = 'Disconnected';
    connectionStatus.className = 'disconnected';
    
    // Attempt to reconnect
    reconnectAttempts++;
    const delay = Math.min(30000, Math.pow(2, reconnectAttempts) * 1000);
    console.log(`Reconnecting in ${delay/1000} seconds...`);
    
    clearTimeout(reconnectTimeout);
    reconnectTimeout = setTimeout(connectWebSocket, delay);
  };
  
  socket.onerror = function(error) {
    console.error('WebSocket error:', error);
  };
  
  socket.onmessage = function(event) {
    console.log('WebSocket message:', event.data);
    handleWebSocketMessage(event.data);
  };
}

// Handle WebSocket messages
function handleWebSocketMessage(data) {
  try {
    const message = JSON.parse(data);
    
    // Update firmware information
    if (message.firmware_version) {
      currentVersion.textContent = message.firmware_version;
      footerVersion.textContent = message.firmware_version;
    }
    
    if (message.latest_version) {
      latestVersion.textContent = message.latest_version || 'Unknown';
    }
    
    // Update status
    if (message.update_status) {
      updateStatusUI(message.update_status, message.update_progress, message.update_error);
    }
    
    // Enable/disable update button
    updateBtn.disabled = !message.update_available;
    
  } catch (error) {
    console.error('Error parsing WebSocket message:', error);
  }
}

// Fetch device information from API
function fetchDeviceInfo() {
  fetch(`${apiBaseUrl}/api/info`)
    .then(response => response.json())
    .then(data => {
      deviceInfo = data;
      
      // Update UI with device info
      deviceName.textContent = data.device || 'Unknown';
      ipAddress.textContent = data.ip || 'Unknown';
      macAddress.textContent = data.mac || 'Unknown';
      currentVersion.textContent = data.firmware_version || 'Unknown';
      footerVersion.textContent = data.firmware_version || 'Unknown';
      latestVersion.textContent = data.latest_version || 'Unknown';
      
      // Format uptime
      const uptimeSeconds = data.uptime || 0;
      uptime.textContent = formatUptime(uptimeSeconds);
      
      // Format memory
      freeMemory.textContent = formatBytes(data.free_heap || 0);
      
      // Format flash size
      flashSize.textContent = formatBytes(data.flash_size || 0);
      
      // Update status
      updateStatusUI(data.update_status, data.update_progress, data.update_error);
      
      // Enable/disable update button
      updateBtn.disabled = !data.update_available;
    })
    .catch(error => {
      console.error('Error fetching device info:', error);
      connectionStatus.textContent = 'API Error';
      connectionStatus.className = 'disconnected';
    });
}

// Check for updates
function checkForUpdates() {
  checkUpdatesBtn.disabled = true;
  updateStatusUI('checking');
  
  fetch(`${apiBaseUrl}/api/check`)
    .then(response => response.json())
    .then(data => {
      console.log('Check response:', data);
      
      if (data.update_available) {
        latestVersion.textContent = data.latest_version || 'Unknown';
        updateBtn.disabled = false;
      } else {
        updateBtn.disabled = true;
      }
    })
    .catch(error => {
      console.error('Error checking for updates:', error);
      updateStatusUI('error', 0, 'Failed to check for updates');
    })
    .finally(() => {
      checkUpdatesBtn.disabled = false;
    });
}

// Start firmware update
function startUpdate() {
  if (!confirm('Are you sure you want to update the firmware? The device will restart after the update.')) {
    return;
  }
  
  updateBtn.disabled = true;
  checkUpdatesBtn.disabled = true;
  updateStatusUI('updating', 0);
  
  fetch(`${apiBaseUrl}/api/update`)
    .then(response => response.json())
    .then(data => {
      console.log('Update response:', data);
      
      if (!data.success) {
        updateStatusUI('error', 0, data.error || 'Failed to start update');
        updateBtn.disabled = false;
        checkUpdatesBtn.disabled = false;
      }
    })
    .catch(error => {
      console.error('Error starting update:', error);
      updateStatusUI('error', 0, 'Failed to start update');
      updateBtn.disabled = false;
      checkUpdatesBtn.disabled = false;
    });
}

// Restart device
function restartDevice() {
  if (!confirm('Are you sure you want to restart the device?')) {
    return;
  }
  
  fetch(`${apiBaseUrl}/api/restart`)
    .then(response => response.json())
    .then(data => {
      console.log('Restart response:', data);
      alert('Device is restarting...');
    })
    .catch(error => {
      console.error('Error restarting device:', error);
      alert('Failed to restart device');
    });
}

// Update the UI based on status
function updateStatusUI(status, progress, error) {
  // Update status class
  updateStatus.className = `status-${status}`;
  
  // Update status text
  const statusText = updateStatus.querySelector('.status-text');
  switch (status) {
    case 'idle':
      statusText.textContent = 'Idle';
      break;
    case 'checking':
      statusText.textContent = 'Checking for updates...';
      break;
    case 'updating':
      statusText.textContent = 'Updating firmware...';
      break;
    case 'success':
      statusText.textContent = 'Update successful!';
      break;
    case 'error':
      statusText.textContent = 'Update failed';
      break;
    default:
      statusText.textContent = status;
  }
  
  // Show/hide progress bar
  if (status === 'updating' && typeof progress !== 'undefined') {
    progressContainer.classList.remove('hidden');
    progressBarInner.style.width = `${progress}%`;
    progressText.textContent = `${progress}%`;
  } else {
    progressContainer.classList.add('hidden');
  }
  
  // Show/hide error message
  if (status === 'error' && error) {
    errorMessage.classList.remove('hidden');
    errorMessage.textContent = error;
  } else {
    errorMessage.classList.add('hidden');
  }
}

// Format uptime in human-readable format
function formatUptime(seconds) {
  const days = Math.floor(seconds / 86400);
  seconds %= 86400;
  const hours = Math.floor(seconds / 3600);
  seconds %= 3600;
  const minutes = Math.floor(seconds / 60);
  seconds %= 60;
  
  let result = '';
  if (days > 0) {
    result += `${days}d `;
  }
  if (hours > 0 || days > 0) {
    result += `${hours}h `;
  }
  if (minutes > 0 || hours > 0 || days > 0) {
    result += `${minutes}m `;
  }
  result += `${seconds}s`;
  
  return result;
}

// Format bytes in human-readable format
function formatBytes(bytes, decimals = 2) {
  if (bytes === 0) return '0 Bytes';
  
  const k = 1024;
  const dm = decimals < 0 ? 0 : decimals;
  const sizes = ['Bytes', 'KB', 'MB', 'GB', 'TB'];
  
  const i = Math.floor(Math.log(bytes) / Math.log(k));
  
  return parseFloat((bytes / Math.pow(k, i)).toFixed(dm)) + ' ' + sizes[i];
}

// Initialize the application when the page loads
document.addEventListener('DOMContentLoaded', init);