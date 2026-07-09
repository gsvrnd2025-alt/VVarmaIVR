/**
 * V-VARMA Visual IVR Flow Builder & Simulator Engine
 * Author: Antigravity AI
 * Theme: Varma Premium IoT Dark Dashboard
 */

// Global Flow Variables
let ivrFlow = {
  nodes: {
    "start": {
      id: "start",
      name: "Start Call",
      type: "play_voice",
      audio: "/01/001.wav",
      delay_before: 0,
      delay_after: 1000,
      interruptible: true,
      next: "main_menu",
      x: 100,
      y: 200,
      color: "#2ecc71"
    },
    "main_menu": {
      id: "main_menu",
      name: "Main Menu",
      type: "wait_dtmf",
      audio: "/01/005.wav",
      timeout: 5,
      max_attempts: 2,
      interruptible: true,
      options: {
        "1": "warranty_check",
        "2": "complaint_reg_step",
        "3": "agent_connect_step"
      },
      fail_node: "repeat_prompt",
      x: 350,
      y: 200,
      color: "#3b82f6"
    },
    "warranty_check": {
      id: "warranty_check",
      name: "Warranty Check",
      type: "warranty_verify",
      audio: "/01/006.wav",
      success_node: "warranty_valid_sms",
      fail_node: "warranty_invalid_prompt",
      claimed_node: "warranty_claimed_prompt",
      x: 650,
      y: 50,
      color: "#f18912"
    },
    "warranty_valid_sms": {
      id: "warranty_valid_sms",
      name: "Send Valid SMS",
      type: "send_sms",
      sms_template: "Dear customer, your warranty for {Product} ({Serial}) is VALID. Status: {Warranty}.",
      next: "hangup_step",
      x: 950,
      y: 20,
      color: "#a855f7"
    },
    "warranty_invalid_prompt": {
      id: "warranty_invalid_prompt",
      name: "Invalid Warranty",
      type: "play_voice",
      audio: "/01/009.wav",
      next: "hangup_step",
      x: 950,
      y: 120,
      color: "#ef4444"
    },
    "warranty_claimed_prompt": {
      id: "warranty_claimed_prompt",
      name: "Claimed Warranty",
      type: "play_voice",
      audio: "/01/008.wav",
      next: "hangup_step",
      x: 950,
      y: 220,
      color: "#f97316"
    },
    "complaint_reg_step": {
      id: "complaint_reg_step",
      name: "Register Complaint",
      type: "complaint_reg",
      audio: "/01/010.wav",
      next: "complaint_success_sms",
      x: 650,
      y: 350,
      color: "#00b0ff"
    },
    "complaint_success_sms": {
      id: "complaint_success_sms",
      name: "Send Complaint SMS",
      type: "send_sms",
      sms_template: "Complaint registered successfully! Ticket: {ComplaintNo}. We will update you soon.",
      next: "hangup_step",
      x: 950,
      y: 350,
      color: "#a855f7"
    },
    "agent_connect_step": {
      id: "agent_connect_step",
      name: "Connect to Care",
      type: "agent_connect",
      audio: "/01/015.wav",
      x: 650,
      y: 500,
      color: "#e63946"
    },
    "repeat_prompt": {
      id: "repeat_prompt",
      name: "Repeat Option",
      type: "play_voice",
      audio: "/01/017.wav",
      next: "main_menu",
      x: 350,
      y: 480,
      color: "#f97316"
    },
    "hangup_step": {
      id: "hangup_step",
      name: "Hang Up",
      type: "hangup",
      x: 1200,
      y: 200,
      color: "#747d8c"
    }
  }
};

let activeDragNode = null;
let dragOffset = { x: 0, y: 0 };
let currentSelectedNode = null;
let svgCanvas = null;
let scale = 1.0;
let voiceFiles = []; // Populated from /sd_files scan
let smsTemplates = []; // Manage SMS templates

// Panning and connection drawing variables
let isPanning = false;
let panStart = { x: 0, y: 0 };
let panOffset = { x: 0, y: 0 };
let isDrawingConnection = false;
let connectionSourceNodeId = null;

let currentToolMode = 'pointer'; // 'pointer' or 'line'
let selectedConnection = null;   // { fromId, toId, label }

// Custom Variable list for visual help
const IVR_VARIABLES = [
  "Name", "Phone", "ComplaintNo", "Dealer", "Address",
  "Product", "Model", "Serial", "Warranty", "Status",
  "HelpNumber", "CurrentDate", "CurrentTime"
];

// Initialize Flow Builder components
function initFlowBuilder() {
  svgCanvas = document.getElementById("flow-svg-canvas");
  const canvasArea = document.getElementById("flow-canvas-area");

  if (!canvasArea) return;

  // Track dragging/drawing on canvas
  canvasArea.addEventListener("mousedown", (e) => {
    // Check if drawing line tool is active
    if (currentToolMode === 'line') {
      const nodeEl = e.target.closest(".flow-node");
      if (nodeEl) {
        e.stopPropagation();
        e.preventDefault();
        const nodeId = nodeEl.dataset.id;
        
        if (!connectionSourceNodeId) {
          connectionSourceNodeId = nodeId;
          nodeEl.classList.add("source-highlight");
          showToast("Line Mode", "Click on target node to connect.", "info");
        } else {
          const targetNodeId = nodeId;
          if (targetNodeId !== connectionSourceNodeId) {
            handleConnectNodes(connectionSourceNodeId, targetNodeId, e.clientX, e.clientY);
          }
          // Reset
          document.querySelectorAll(".flow-node").forEach(el => el.classList.remove("source-highlight"));
          connectionSourceNodeId = null;
        }
      } else {
        if (connectionSourceNodeId) {
          document.querySelectorAll(".flow-node").forEach(el => el.classList.remove("source-highlight"));
          connectionSourceNodeId = null;
          showToast("Line Mode", "Connection cancelled", "info");
        }
      }
      return;
    }

    // 1. Drag connection line from output port
    if (e.target.classList.contains("output-port")) {
      isDrawingConnection = true;
      connectionSourceNodeId = e.target.dataset.nodeId;
      e.stopPropagation();
      e.preventDefault();
    }
    // 2. Drag node header
    else if (e.target.classList.contains("flow-node-header") || e.target.closest(".flow-node-header")) {
      const nodeEl = e.target.closest(".flow-node");
      const nodeId = nodeEl.dataset.id;
      activeDragNode = ivrFlow.nodes[nodeId];
      const rect = nodeEl.getBoundingClientRect();
      dragOffset.x = (e.clientX - rect.left) / scale;
      dragOffset.y = (e.clientY - rect.top) / scale;
      currentSelectedNode = activeDragNode;
      selectNodeVisual(nodeId);
      
      // Clear connection selection when selecting node
      selectedConnection = null;
      redrawConnections();
      updateDeleteButtonState();
      
      e.stopPropagation();
    }
    // 3. Pan canvas background
    else {
      isPanning = true;
      panStart.x = e.clientX - panOffset.x;
      panStart.y = e.clientY - panOffset.y;
      
      // Clear selections when clicking background
      currentSelectedNode = null;
      selectedConnection = null;
      selectNodeVisual("");
      redrawConnections();
      updateDeleteButtonState();
    }
  });

  document.addEventListener("mousemove", (e) => {
    if (activeDragNode) {
      const parentRect = canvasArea.getBoundingClientRect();
      // Account for zoom (scale) and canvas translation (panOffset) when placing dragged node
      const newX = Math.round((e.clientX - parentRect.left) / scale - dragOffset.x - panOffset.x / scale);
      const newY = Math.round((e.clientY - parentRect.top) / scale - dragOffset.y - panOffset.y / scale);
      
      activeDragNode.x = Math.max(10, Math.min(3000, newX));
      activeDragNode.y = Math.max(10, Math.min(2000, newY));
      
      const nodeEl = document.getElementById(`node-${activeDragNode.id}`);
      if (nodeEl) {
        nodeEl.style.left = `${activeDragNode.x}px`;
        nodeEl.style.top = `${activeDragNode.y}px`;
      }
      redrawConnections();
    } 
    else if (isDrawingConnection) {
      const sourceNode = ivrFlow.nodes[connectionSourceNodeId];
      const sourceEl = document.getElementById(`node-${connectionSourceNodeId}`);
      if (sourceNode && sourceEl) {
        const parentRect = canvasArea.getBoundingClientRect();
        const startX = sourceNode.x + sourceEl.offsetWidth;
        const startY = sourceNode.y + (sourceEl.offsetHeight / 2);
        
        // Translate mouse coords to scaled/translated canvas coordinate space
        const endX = (e.clientX - parentRect.left) / scale - panOffset.x / scale;
        const endY = (e.clientY - parentRect.top) / scale - panOffset.y / scale;
        
        drawTempConnectionLine(startX, startY, endX, endY);
      }
    }
    else if (isPanning) {
      panOffset.x = e.clientX - panStart.x;
      panOffset.y = e.clientY - panStart.y;
      updateCanvasTransform();
    }
  });

  document.addEventListener("mouseup", (e) => {
    if (activeDragNode) {
      activeDragNode = null;
      saveFlowLocal(); // Save current workspace layout
    }
    if (isDrawingConnection) {
      isDrawingConnection = false;
      const tempLine = document.getElementById("temp-connection-line");
      if (tempLine) tempLine.remove();
      
      // Connect to target node if dropped over one
      const targetNodeEl = e.target.closest(".flow-node");
      if (targetNodeEl) {
        const targetNodeId = targetNodeEl.dataset.id;
        if (targetNodeId !== connectionSourceNodeId) {
          handleConnectNodes(connectionSourceNodeId, targetNodeId, e.clientX, e.clientY);
        }
      }
    }
    if (isPanning) {
      isPanning = false;
    }
  });

  // Zoom controls
  document.getElementById("btn-zoom-in")?.addEventListener("click", () => adjustZoom(0.1));
  document.getElementById("btn-zoom-out")?.addEventListener("click", () => adjustZoom(-0.1));
  document.getElementById("btn-zoom-reset")?.addEventListener("click", () => adjustZoom(1.0, true));

  // Add block events
  document.querySelectorAll(".toolbar-block-btn").forEach(btn => {
    btn.addEventListener("click", () => {
      addNewNode(btn.dataset.type);
    });
  });

  // Editor Tools Selection Handlers
  const btnPointer = document.getElementById("btn-tool-pointer");
  const btnLine = document.getElementById("btn-tool-line");
  const btnDelete = document.getElementById("btn-tool-delete");

  if (btnPointer && btnLine) {
    btnPointer.addEventListener("click", () => {
      currentToolMode = "pointer";
      btnPointer.classList.add("active", "btn-glass-primary");
      btnPointer.classList.remove("btn-glass-secondary");
      btnLine.classList.remove("active", "btn-glass-primary");
      btnLine.classList.add("btn-glass-secondary");
      
      // Clear line drawing highlight
      if (connectionSourceNodeId) {
        document.querySelectorAll(".flow-node").forEach(el => el.classList.remove("source-highlight"));
        connectionSourceNodeId = null;
      }
    });

    btnLine.addEventListener("click", () => {
      currentToolMode = "line";
      btnLine.classList.add("active", "btn-glass-primary");
      btnLine.classList.remove("btn-glass-secondary");
      btnPointer.classList.remove("active", "btn-glass-primary");
      btnPointer.classList.add("btn-glass-secondary");
      
      // Clear connection selection when changing tool mode
      selectedConnection = null;
      redrawConnections();
      updateDeleteButtonState();
    });
  }

  if (btnDelete) {
    btnDelete.addEventListener("click", () => {
      handleDeleteSelection();
    });
  }

  // Keyboard Delete key handler
  document.addEventListener("keydown", (e) => {
    if (e.key === "Delete" || e.key === "Backspace") {
      if (document.activeElement.tagName !== "INPUT" && document.activeElement.tagName !== "TEXTAREA") {
        handleDeleteSelection();
      }
    }
  });

  loadFlowLocal();
  scanVoiceLibrary();
  loadSmsTemplates();
  renderFlow();
}

function adjustZoom(amount, reset = false) {
  if (reset) {
    scale = 1.0;
    panOffset = { x: 0, y: 0 };
  } else {
    scale = Math.max(0.4, Math.min(1.8, scale + amount));
  }
  updateCanvasTransform();
}

function updateCanvasTransform() {
  const container = document.getElementById("flow-nodes-container");
  if (container) {
    container.style.transform = `translate(${panOffset.x}px, ${panOffset.y}px) scale(${scale})`;
    container.style.transformOrigin = "0 0";
  }
  if (svgCanvas) {
    svgCanvas.style.transform = `translate(${panOffset.x}px, ${panOffset.y}px) scale(${scale})`;
    svgCanvas.style.transformOrigin = "0 0";
  }
  document.getElementById("zoom-indicator").textContent = `${Math.round(scale * 100)}%`;
  redrawConnections();
}

// Temporary connection line during dragging from output port
function drawTempConnectionLine(x1, y1, x2, y2) {
  let tempLine = document.getElementById("temp-connection-line");
  if (!tempLine) {
    tempLine = document.createElementNS("http://www.w3.org/2000/svg", "path");
    tempLine.setAttribute("id", "temp-connection-line");
    tempLine.setAttribute("stroke", "#2ecc71");
    tempLine.setAttribute("stroke-width", "3");
    tempLine.setAttribute("stroke-dasharray", "5,5");
    tempLine.setAttribute("fill", "none");
    svgCanvas.appendChild(tempLine);
  }
  const dx = Math.abs(x2 - x1) * 0.4;
  const dPath = `M ${x1} ${y1} C ${x1 + dx} ${y1}, ${x2 - dx} ${y2}, ${x2} ${y2}`;
  tempLine.setAttribute("d", dPath);
}

// Visual connection linker drop handler
function handleConnectNodes(fromId, toId, clientX, clientY) {
  const fromNode = ivrFlow.nodes[fromId];
  if (!fromNode) return;

  const options = [];
  
  if (fromNode.type === "play_voice" || fromNode.type === "delay" || fromNode.type === "send_sms" || fromNode.type === "complaint_reg" || fromNode.type === "installation_reg") {
    options.push({ label: "Next Step", action: () => { fromNode.next = toId; saveAndRedraw(); } });
  }
  else if (fromNode.type === "wait_dtmf") {
    options.push({ label: "Fail / Timeout Node", action: () => { fromNode.fail_node = toId; saveAndRedraw(); } });
    for (let i = 0; i <= 9; i++) {
      options.push({ label: `DTMF key ${i}`, action: () => {
        if (!fromNode.options) fromNode.options = {};
        fromNode.options[String(i)] = toId;
        saveAndRedraw();
      } });
    }
  }
  else if (fromNode.type === "warranty_verify") {
    options.push({ label: "Success Node", action: () => { fromNode.success_node = toId; saveAndRedraw(); } });
    options.push({ label: "Fail Node", action: () => { fromNode.fail_node = toId; saveAndRedraw(); } });
    options.push({ label: "Claimed Node", action: () => { fromNode.claimed_node = toId; saveAndRedraw(); } });
  }

  if (options.length === 0) return;

  if (options.length === 1) {
    options[0].action();
    return;
  }

  showConnectionChoiceMenu(options, clientX, clientY);
}

function saveAndRedraw() {
  saveFlowLocal();
  renderFlow();
}

function showConnectionChoiceMenu(options, x, y) {
  document.getElementById("connection-choice-menu")?.remove();

  const menu = document.createElement("div");
  menu.id = "connection-choice-menu";
  menu.className = "glass-card position-fixed p-2 shadow-lg";
  menu.style.left = `${x}px`;
  menu.style.top = `${y}px`;
  menu.style.zIndex = "99999";
  menu.style.minWidth = "180px";
  menu.style.border = "1px solid var(--primary-accent)";
  menu.style.maxHeight = "300px";
  menu.style.overflowY = "auto";

  const title = document.createElement("div");
  title.className = "text-muted small fw-bold mb-2 pb-1 border-bottom border-secondary text-center";
  title.textContent = "SELECT CONNECTION TYPE";
  menu.appendChild(title);

  options.forEach(opt => {
    const btn = document.createElement("button");
    btn.className = "btn btn-glass-secondary btn-xs w-100 text-start py-1 px-2 mb-1 text-white border-0";
    btn.style.fontSize = "11px";
    btn.innerHTML = `<i class="fa-solid fa-link text-warning me-1"></i> ${opt.label}`;
    btn.onclick = () => {
      opt.action();
      menu.remove();
    };
    menu.appendChild(btn);
  });

  const cancelBtn = document.createElement("button");
  cancelBtn.className = "btn btn-danger btn-xs w-100 py-1 mt-1 border-0";
  cancelBtn.style.fontSize = "10px";
  cancelBtn.textContent = "Cancel";
  cancelBtn.onclick = () => menu.remove();
  menu.appendChild(cancelBtn);

  document.body.appendChild(menu);

  const clickOutside = (e) => {
    if (!menu.contains(e.target)) {
      menu.remove();
      document.removeEventListener("mousedown", clickOutside);
    }
  };
  setTimeout(() => document.addEventListener("mousedown", clickOutside), 50);
}

// Draw connection curves in SVG canvas
function redrawConnections() {
  if (!svgCanvas) return;
  svgCanvas.innerHTML = ""; // Clear
  
  // Create definitions for arrow markers
  const defs = document.createElementNS("http://www.w3.org/2000/svg", "defs");
  const marker = document.createElementNS("http://www.w3.org/2000/svg", "marker");
  marker.setAttribute("id", "arrow");
  marker.setAttribute("viewBox", "0 0 10 10");
  marker.setAttribute("refX", "6");
  marker.setAttribute("refY", "5");
  marker.setAttribute("markerWidth", "6");
  marker.setAttribute("markerHeight", "6");
  marker.setAttribute("orient", "auto-start-reverse");
  const path = document.createElementNS("http://www.w3.org/2000/svg", "path");
  path.setAttribute("d", "M 0 0 L 10 5 L 0 10 z");
  path.setAttribute("fill", "#f18912");
  marker.appendChild(path);
  defs.appendChild(marker);
  svgCanvas.appendChild(defs);

  // Trace connections from node definitions
  for (const [id, node] of Object.entries(ivrFlow.nodes)) {
    const fromEl = document.getElementById(`node-${id}`);
    if (!fromEl) continue;

    const fromRect = {
      x: node.x,
      y: node.y,
      w: fromEl.offsetWidth,
      h: fromEl.offsetHeight
    };

    // 1. Trace sequential connection ("next")
    if (node.next) {
      drawCurve(id, node.next, "Next", fromRect);
    }
    
    // 2. Trace options (DTMF branches)
    if (node.options) {
      for (const [key, target] of Object.entries(node.options)) {
        drawCurve(id, target, `Key ${key}`, fromRect);
      }
    }

    // 3. Trace fail/timeout/claimed node connections
    if (node.fail_node) {
      drawCurve(id, node.fail_node, "Fail / Timeout", fromRect);
    }
    if (node.success_node) {
      drawCurve(id, node.success_node, "Success", fromRect);
    }
    if (node.claimed_node) {
      drawCurve(id, node.claimed_node, "Claimed", fromRect);
    }
  }
}

function drawCurve(fromId, toId, label, fromRect) {
  const toNode = ivrFlow.nodes[toId];
  const toEl = document.getElementById(`node-${toId}`);
  if (!toNode || !toEl) return;

  const toRect = {
    x: toNode.x,
    y: toNode.y,
    w: toEl.offsetWidth,
    h: toEl.offsetHeight
  };

  // Connection point calculations (Exit right, Enter left)
  const x1 = fromRect.x + fromRect.w;
  const y1 = fromRect.y + (fromRect.h / 2);
  const x2 = toRect.x;
  const y2 = toRect.y + (toRect.h / 2);

  const dx = Math.abs(x2 - x1) * 0.4;
  const dPath = `M ${x1} ${y1} C ${x1 + dx} ${y1}, ${x2 - dx} ${y2}, ${x2} ${y2}`;

  const g = document.createElementNS("http://www.w3.org/2000/svg", "g");
  g.setAttribute("class", "flow-connection-group");
  
  // Check if this connection is currently selected
  const isSelected = selectedConnection && 
                     selectedConnection.fromId === fromId && 
                     selectedConnection.toId === toId && 
                     selectedConnection.label === label;

  const color = isSelected ? "#ef4444" : "#f18912"; // Red if selected, otherwise Orange
  const strokeWidth = isSelected ? "4" : "2";

  // Thick transparent click target overlay path
  const clickTarget = document.createElementNS("http://www.w3.org/2000/svg", "path");
  clickTarget.setAttribute("d", dPath);
  clickTarget.setAttribute("stroke", "transparent");
  clickTarget.setAttribute("stroke-width", "12");
  clickTarget.setAttribute("fill", "none");
  clickTarget.setAttribute("style", "cursor: pointer;");
  
  clickTarget.addEventListener("click", (e) => {
    e.stopPropagation();
    selectedConnection = { fromId, toId, label };
    currentSelectedNode = null;
    selectNodeVisual("");
    redrawConnections();
    updateDeleteButtonState();
  });
  
  g.appendChild(clickTarget);

  // Visible line path
  const path = document.createElementNS("http://www.w3.org/2000/svg", "path");
  path.setAttribute("d", dPath);
  path.setAttribute("stroke", color);
  path.setAttribute("stroke-width", strokeWidth);
  path.setAttribute("fill", "none");
  path.setAttribute("marker-end", "url(#arrow)");
  g.appendChild(path);

  // Label bubble in the middle
  const mx = (x1 + x2) / 2;
  const my = (y1 + y2) / 2;

  const text = document.createElementNS("http://www.w3.org/2000/svg", "text");
  text.setAttribute("x", mx);
  text.setAttribute("y", my + 4);
  text.setAttribute("fill", "#cbd5e1");
  text.setAttribute("font-size", "10px");
  text.setAttribute("font-weight", "bold");
  text.setAttribute("text-anchor", "middle");
  text.textContent = label;

  const rect = document.createElementNS("http://www.w3.org/2000/svg", "rect");
  rect.setAttribute("x", mx - (label.length * 3.5) - 4);
  rect.setAttribute("y", my - 8);
  rect.setAttribute("width", (label.length * 7) + 8);
  rect.setAttribute("height", "16");
  rect.setAttribute("fill", "#1a1008");
  rect.setAttribute("rx", "4");
  rect.setAttribute("stroke", isSelected ? "rgba(239,68,68,0.4)" : "rgba(241,137,18,0.2)");
  rect.setAttribute("stroke-width", "1");
  rect.setAttribute("style", "cursor: pointer;");
  
  rect.addEventListener("click", (e) => {
    e.stopPropagation();
    selectedConnection = { fromId, toId, label };
    currentSelectedNode = null;
    selectNodeVisual("");
    redrawConnections();
    updateDeleteButtonState();
  });

  g.appendChild(rect);
  g.appendChild(text);
  svgCanvas.appendChild(g);
}

// Render node list inside visual editor canvas
function renderFlow() {
  const container = document.getElementById("flow-nodes-container");
  if (!container) return;
  container.innerHTML = ""; // Clear

  for (const [id, node] of Object.entries(ivrFlow.nodes)) {
    const el = document.createElement("div");
    el.className = "flow-node glass-card";
    el.id = `node-${id}`;
    el.dataset.id = id;
    el.style.left = `${node.x}px`;
    el.style.top = `${node.y}px`;
    el.style.borderColor = node.color || "rgba(241,137,18,0.2)";

    let detailsStr = "";
    if (node.audio) detailsStr += `<div class="node-meta"><i class="fa-solid fa-volume-high me-1 text-primary"></i> ${node.audio.substring(node.audio.lastIndexOf('/')+1)}</div>`;
    if (node.type === "wait_dtmf") detailsStr += `<div class="node-meta text-info"><i class="fa-solid fa-calculator me-1"></i> Timeout: ${node.timeout}s</div>`;
    if (node.type === "delay") detailsStr += `<div class="node-meta text-info"><i class="fa-solid fa-hourglass me-1"></i> Delay: ${node.delay_after}ms</div>`;
    if (node.type === "send_sms") detailsStr += `<div class="node-meta text-success"><i class="fa-solid fa-comment-sms me-1"></i> SMS Enabled</div>`;

    el.innerHTML = `
      <div class="flow-node-header d-flex justify-content-between align-items-center">
        <span class="fw-bold text-white small"><i class="fa-solid ${getNodeIcon(node.type)} me-1" style="color: ${node.color || 'var(--primary-accent)'}"></i> ${node.name}</span>
        <div class="node-actions-menu d-flex align-items-center">
          <button class="btn btn-link btn-xs p-0 text-muted hover-white" title="Edit" onclick="editNodeProperties('${id}')"><i class="fa-solid fa-pen" style="font-size:0.7rem;"></i></button>
          <button class="btn btn-link btn-xs p-0 text-info ms-1 hover-white" title="Duplicate" onclick="duplicateNode('${id}')"><i class="fa-solid fa-clone" style="font-size:0.7rem;"></i></button>
          <button class="btn btn-link btn-xs p-0 text-warning ms-1 hover-white" title="Copy" onclick="copyNode('${id}')"><i class="fa-solid fa-copy" style="font-size:0.7rem;"></i></button>
          <button class="btn btn-link btn-xs p-0 text-danger ms-1" title="Delete" onclick="deleteNode('${id}')"><i class="fa-solid fa-trash" style="font-size:0.7rem;"></i></button>
        </div>
      </div>
      <div class="flow-node-body mt-2">
        ${detailsStr}
      </div>
      <div class="input-port"></div>
      <div class="output-port"></div>
    `;

    container.appendChild(el);
  }

  setTimeout(redrawConnections, 50); // Redraw curves after DOM layout settles
}

function getNodeIcon(type) {
  switch (type) {
    case "play_voice": return "fa-volume-high";
    case "wait_dtmf":  return "fa-keyboard";
    case "delay":      return "fa-hourglass-half";
    case "send_sms":   return "fa-envelope";
    case "customer_verify": return "fa-user-check";
    case "warranty_verify": return "fa-shield-halved";
    case "complaint_reg":   return "fa-ticket-simple";
    case "installation_reg": return "fa-wrench";
    case "agent_connect":   return "fa-headset";
    case "hangup":     return "fa-phone-slash";
    default:           return "fa-square";
  }
}

function selectNodeVisual(nodeId) {
  document.querySelectorAll(".flow-node").forEach(el => {
    el.classList.remove("selected");
  });
  document.getElementById(`node-${nodeId}`)?.classList.add("selected");
}

// Add new visual step/block
function addNewNode(type) {
  const id = `step_${Date.now()}`;
  const names = {
    play_voice: "Play Voice",
    wait_dtmf: "Wait DTMF",
    delay: "Delay",
    send_sms: "Send SMS",
    customer_verify: "Verify Customer",
    warranty_verify: "Check Warranty",
    complaint_reg: "Log Complaint",
    installation_reg: "Book Install",
    agent_connect: "Transfer Call",
    hangup: "Hang Up"
  };

  const colors = {
    play_voice: "#f18912",
    wait_dtmf: "#3b82f6",
    delay: "#29c7ff",
    send_sms: "#2ecc71",
    customer_verify: "#f97316",
    warranty_verify: "#2ecc71",
    complaint_reg: "#e84393",
    installation_reg: "#00b0ff",
    agent_connect: "#e63946",
    hangup: "#747d8c"
  };

  ivrFlow.nodes[id] = {
    id: id,
    name: names[type],
    type: type,
    x: 400,
    y: 300,
    color: colors[type],
    audio: "",
    delay_before: 0,
    delay_after: 0
  };

  renderFlow();
  saveFlowLocal();
}

function deleteNode(id) {
  if (id === "start") {
    showToast("Error", "Start node cannot be deleted", "danger");
    return;
  }
  
  // Clean connections to this node
  for (const [key, node] of Object.entries(ivrFlow.nodes)) {
    if (node.next === id) node.next = "";
    if (node.fail_node === id) node.fail_node = "";
    if (node.success_node === id) node.success_node = "";
    if (node.claimed_node === id) node.claimed_node = "";
    if (node.options) {
      for (const [oKey, target] of Object.entries(node.options)) {
        if (target === id) delete node.options[oKey];
      }
    }
  }

  delete ivrFlow.nodes[id];
  renderFlow();
  saveFlowLocal();
}

// Copy node properties to clipboard
let copiedNodeData = null;

function copyNode(id) {
  const node = ivrFlow.nodes[id];
  if (!node) return;
  copiedNodeData = JSON.parse(JSON.stringify(node));
  showToast("Copy Flow Block", `Block "${node.name}" properties copied.`, "success");
}

// Paste properties or duplicate node
function duplicateNode(id) {
  const node = ivrFlow.nodes[id];
  if (!node) return;
  
  const newId = `step_${Date.now()}`;
  const newNode = JSON.parse(JSON.stringify(node));
  newNode.id = newId;
  newNode.name = `${node.name} (Copy)`;
  newNode.x = node.x + 50;
  newNode.y = node.y + 50;
  
  // Clear connections to avoid overlap/cycle errors
  delete newNode.next;
  delete newNode.options;
  delete newNode.fail_node;
  delete newNode.success_node;
  delete newNode.claimed_node;
  
  ivrFlow.nodes[newId] = newNode;
  saveFlowLocal();
  renderFlow();
  showToast("Duplicate Block", `Duplicated block to "${newNode.name}".`, "success");
}

// Edit properties panel implementation
function editNodeProperties(id) {
  const node = ivrFlow.nodes[id];
  currentSelectedNode = node;
  selectNodeVisual(id);

  const container = document.getElementById("node-editor-form-container");
  if (!container) return;

  let formHtml = `
    <div class="mb-3">
      <label class="form-label text-muted small">Step Name</label>
      <input type="text" id="edit-node-name" class="form-control glass-input" value="${node.name}">
    </div>
    <div class="mb-3">
      <label class="form-label text-muted small">Step Color</label>
      <input type="color" id="edit-node-color" class="form-control glass-input" style="height:40px; padding: 2px;" value="${node.color || '#f18912'}">
    </div>
  `;

  // Dynamic parameters based on block type
  if (node.type === "play_voice" || node.type === "wait_dtmf" || node.type === "warranty_verify" || node.type === "agent_connect") {
    formHtml += `
      <div class="mb-3">
        <label class="form-label text-muted small">Select Voice Clip (.wav)</label>
        <select id="edit-node-audio" class="form-control glass-select">
          <option value="">-- No Audio --</option>
          ${voiceFiles.map(f => `<option value="${f}" ${node.audio === f ? 'selected' : ''}>${f}</option>`).join("")}
        </select>
      </div>
      <div class="row mb-3">
        <div class="col">
          <label class="form-label text-muted small">Delay Before (ms)</label>
          <input type="number" id="edit-node-delay-before" class="form-control glass-input" value="${node.delay_before || 0}">
        </div>
        <div class="col">
          <label class="form-label text-muted small">Delay After (ms)</label>
          <input type="number" id="edit-node-delay-after" class="form-control glass-input" value="${node.delay_after || 0}">
        </div>
      </div>
      <div class="mb-3 form-check form-switch">
        <input type="checkbox" class="form-check-input" id="edit-node-interruptible" ${node.interruptible !== false ? 'checked' : ''}>
        <label class="form-check-label text-muted small" for="edit-node-interruptible">Allow Interrupt (DTMF during play)</label>
      </div>
    `;
  }

  if (node.type === "wait_dtmf") {
    formHtml += `
      <div class="row mb-3">
        <div class="col">
          <label class="form-label text-muted small">Timeout (sec)</label>
          <input type="number" id="edit-node-timeout" class="form-control glass-input" value="${node.timeout || 5}">
        </div>
        <div class="col">
          <label class="form-label text-muted small">Max Attempts</label>
          <input type="number" id="edit-node-max-attempts" class="form-control glass-input" value="${node.max_attempts || 2}">
        </div>
      </div>
      <h6 class="text-white mt-4 border-bottom pb-2 border-secondary small">DTMF Keys Configuration</h6>
      <div id="dtmf-options-list" class="mb-3">
        <!-- DTMF keys 0-9, *, # mappings -->
      </div>
      <button type="button" class="btn btn-xs btn-glass-secondary mb-3" onclick="addDtmfMapping()"><i class="fa-solid fa-plus me-1"></i> Add Key Mapping</button>
      
      <div class="mb-3">
        <label class="form-label text-muted small">Timeout / Failed Action (Go to Step)</label>
        <select id="edit-node-fail-node" class="form-control glass-select">
          <option value="">-- Hang Up --</option>
          ${getOtherNodesOptions(node.id, node.fail_node)}
        </select>
      </div>
    `;
  }

  if (node.type === "play_voice" || node.type === "delay" || node.type === "send_sms" || node.type === "complaint_reg" || node.type === "installation_reg") {
    formHtml += `
      <div class="mb-3">
        <label class="form-label text-muted small">Next Step</label>
        <select id="edit-node-next" class="form-control glass-select">
          <option value="">-- Hang Up --</option>
          ${getOtherNodesOptions(node.id, node.next)}
        </select>
      </div>
    `;
  }

  if (node.type === "send_sms") {
    formHtml += `
      <div class="mb-3">
        <label class="form-label text-muted small">SMS Template</label>
        <textarea id="edit-node-sms-template" class="form-control glass-input" rows="4" placeholder="Enter SMS message body">${node.sms_template || ""}</textarea>
        <span class="text-muted" style="font-size:0.72rem;">Variables: ${IVR_VARIABLES.map(v => `{${v}}`).join(", ")}</span>
      </div>
    `;
  }

  if (node.type === "customer_verify") {
    formHtml += `
      <div class="mb-3">
        <label class="form-label text-muted small">If Found (Go to Step)</label>
        <select id="edit-node-success-node" class="form-control glass-select">
          <option value="">-- Hang Up --</option>
          ${getOtherNodesOptions(node.id, node.success_node)}
        </select>
      </div>
      <div class="mb-3">
        <label class="form-label text-muted small">If Not Found (Go to Step)</label>
        <select id="edit-node-fail-node" class="form-control glass-select">
          <option value="">-- Hang Up --</option>
          ${getOtherNodesOptions(node.id, node.fail_node)}
        </select>
      </div>
    `;
  }

  if (node.type === "warranty_verify") {
    formHtml += `
      <div class="mb-3">
        <label class="form-label text-muted small">If Valid (Go to Step)</label>
        <select id="edit-node-success-node" class="form-control glass-select">
          <option value="">-- Hang Up --</option>
          ${getOtherNodesOptions(node.id, node.success_node)}
        </select>
      </div>
      <div class="mb-3">
        <label class="form-label text-muted small">If Claimed (Go to Step)</label>
        <select id="edit-node-claimed-node" class="form-control glass-select">
          <option value="">-- Hang Up --</option>
          ${getOtherNodesOptions(node.id, node.claimed_node)}
        </select>
      </div>
      <div class="mb-3">
        <label class="form-label text-muted small">If Invalid / Timeout (Go to Step)</label>
        <select id="edit-node-fail-node" class="form-control glass-select">
          <option value="">-- Hang Up --</option>
          ${getOtherNodesOptions(node.id, node.fail_node)}
        </select>
      </div>
    `;
  }

  container.innerHTML = formHtml;

  // If DTMF options list exists, render it
  if (node.type === "wait_dtmf") {
    renderDtmfMappingsList(node);
  }

  // Open property editor sidebar panel
  const panel = document.getElementById("node-editor-panel");
  if (panel) {
    panel.classList.add("open");
  }
}

function getOtherNodesOptions(currentNodeId, selectedTargetId) {
  return Object.values(ivrFlow.nodes)
    .filter(n => n.id !== currentNodeId)
    .map(n => `<option value="${n.id}" ${selectedTargetId === n.id ? 'selected' : ''}>${n.name} (${n.id})</option>`)
    .join("");
}

function renderDtmfMappingsList(node) {
  const container = document.getElementById("dtmf-options-list");
  if (!container) return;
  container.innerHTML = "";

  const options = node.options || {};
  for (const [key, target] of Object.entries(options)) {
    const div = document.createElement("div");
    div.className = "row g-2 mb-2 align-items-center dtmf-mapping-row";
    div.dataset.key = key;
    div.innerHTML = `
      <div class="col-3">
        <input type="text" class="form-control glass-input text-center dtmf-key-val" value="${key}" maxlength="1">
      </div>
      <div class="col-7">
        <select class="form-control glass-select dtmf-target-val">
          <option value="">-- Hang Up --</option>
          ${getOtherNodesOptions(node.id, target)}
        </select>
      </div>
      <div class="col-2 text-end">
        <button type="button" class="btn btn-link text-danger p-0" onclick="this.closest('.row').remove()"><i class="fa-solid fa-trash-can"></i></button>
      </div>
    `;
    container.appendChild(div);
  }
}

function addDtmfMapping() {
  const container = document.getElementById("dtmf-options-list");
  if (!container) return;
  
  const div = document.createElement("div");
  div.className = "row g-2 mb-2 align-items-center dtmf-mapping-row";
  div.innerHTML = `
    <div class="col-3">
      <input type="text" class="form-control glass-input text-center dtmf-key-val" value="" placeholder="Key" maxlength="1">
    </div>
    <div class="col-7">
      <select class="form-control glass-select dtmf-target-val">
        <option value="">-- Hang Up --</option>
        ${getOtherNodesOptions(currentSelectedNode.id, "")}
      </select>
    </div>
    <div class="col-2 text-end">
      <button type="button" class="btn btn-link text-danger p-0" onclick="this.closest('.row').remove()"><i class="fa-solid fa-trash-can"></i></button>
    </div>
  `;
  container.appendChild(div);
}

// Save modified properties back to Node object
function saveNodeProperties() {
  if (!currentSelectedNode) return;
  const id = currentSelectedNode.id;
  const node = ivrFlow.nodes[id];

  node.name = document.getElementById("edit-node-name").value.trim();
  node.color = document.getElementById("edit-node-color").value;

  if (document.getElementById("edit-node-audio")) {
    node.audio = document.getElementById("edit-node-audio").value;
  }
  if (document.getElementById("edit-node-delay-before")) {
    node.delay_before = parseInt(document.getElementById("edit-node-delay-before").value) || 0;
  }
  if (document.getElementById("edit-node-delay-after")) {
    node.delay_after = parseInt(document.getElementById("edit-node-delay-after").value) || 0;
  }
  if (document.getElementById("edit-node-interruptible")) {
    node.interruptible = document.getElementById("edit-node-interruptible").checked;
  }
  if (document.getElementById("edit-node-next")) {
    node.next = document.getElementById("edit-node-next").value;
  }
  if (document.getElementById("edit-node-success-node")) {
    node.success_node = document.getElementById("edit-node-success-node").value;
  }
  if (document.getElementById("edit-node-fail-node")) {
    node.fail_node = document.getElementById("edit-node-fail-node").value;
  }
  if (document.getElementById("edit-node-claimed-node")) {
    node.claimed_node = document.getElementById("edit-node-claimed-node").value;
  }
  if (document.getElementById("edit-node-sms-template")) {
    node.sms_template = document.getElementById("edit-node-sms-template").value;
  }

  // Parse DTMF mappings list
  if (node.type === "wait_dtmf") {
    node.options = {};
    document.querySelectorAll(".dtmf-mapping-row").forEach(row => {
      const key = row.querySelector(".dtmf-key-val").value.trim();
      const target = row.querySelector(".dtmf-target-val").value;
      if (key && target) {
        node.options[key] = target;
      }
    });
  }

  // Close panel
  document.getElementById("node-editor-panel")?.classList.remove("open");
  renderFlow();
  saveFlowLocal();
  showToast("Success", "Step parameters updated successfully", "success");
}

function cancelNodeProperties() {
  document.getElementById("node-editor-panel")?.classList.remove("open");
}

// Load and Save flow visualizations workspace state locally in browser
function saveFlowLocal() {
  localStorage.setItem("vvarma_ivr_flow", JSON.stringify(ivrFlow));
}

function loadFlowLocal() {
  const saved = localStorage.getItem("vvarma_ivr_flow");
  if (saved) {
    try {
      ivrFlow = JSON.parse(saved);
    } catch (e) {
      console.error("Failed to parse saved flow workspace localstorage data");
    }
  }
}

// Scans SD card directories to list WAV files for the visual builder
async function scanVoiceLibrary() {
  try {
    const res = await fetch("/sd_files");
    const files = await res.json();
    voiceFiles = files
      .filter(f => f.name.toLowerCase().endsWith(".wav") || f.name.toLowerCase().endsWith(".mp3"))
      .map(f => f.name);
    console.log("SD voice clips loaded:", voiceFiles.length);
    renderVoiceLibraryUI(files);
  } catch (e) {
    console.error("Failed to retrieve SD voice library list:", e);
  }
}

// Google Sheets Sync SQLite implementation using sql.js WebAssembly
let SQL = null;
async function initSqlJS() {
  if (SQL) return;
  // If offline, SQL.js will be loaded from local files on the SD card
  try {
    SQL = await initSqlJs({
      locateFile: file => `/libs/${file}`
    });
    console.log("SQL.js initialized successfully.");
  } catch (e) {
    console.warn("Local SQL.js loading failed. Trying CDN fallback...", e);
    try {
      SQL = await initSqlJs({
        locateFile: file => `https://cdnjs.cloudflare.com/ajax/libs/sql.js/1.10.3/${file}`
      });
      console.log("SQL.js CDN fallback initialized successfully.");
    } catch (err) {
      showToast("Sync Error", "Failed to load WebAssembly SQLite engine. Sync disabled.", "danger");
      throw err;
    }
  }
}

async function triggerDatabaseSync() {
  const btn = document.getElementById("btn-trigger-sync");
  if (btn) btn.disabled = true;
  showToast("Database Sync", "Initializing SQLite conversion engine...", "info");

  try {
    await initSqlJS();
    
    // Fetch google sheet configuration
    const configRes = await fetch("/api/get-config");
    const config = await configRes.json();
    const scriptId = config.scriptId;
    const sheetId = config.sheetId;

    if (!scriptId) {
      throw new Error("Google Sheets Script ID not configured in settings.");
    }

    showToast("Database Sync", "Downloading customer records from Google Sheets...", "info");
    // Fetch sheets payload directly
    const url = `https://script.google.com/macros/s/${scriptId}/exec?action=pullData&sheetId=${sheetId || '1K8SnQA8H2LVEnxyRTKYfTdLYRe2YHC4qQFqWUyOejQw'}&sheetName=IVR`;
    const res = await fetch(url);
    const data = await res.json(); // should be an array of rows [phone, name, serial, product, warranty, status, etc]

    showToast("Database Sync", "Generating indexed SQLite database binary...", "info");
    const db = new SQL.Database();
    db.run(`
      CREATE TABLE IF NOT EXISTS customers (
        phone TEXT,
        name TEXT,
        serial TEXT PRIMARY KEY,
        product TEXT,
        warranty TEXT
      );
      CREATE INDEX IF NOT EXISTS idx_phone ON customers(phone);
    `);

    // Insert sheet rows
    db.run("BEGIN TRANSACTION;");
    const stmt = db.prepare("INSERT OR REPLACE INTO customers (phone, name, serial, product, warranty) VALUES (?, ?, ?, ?, ?)");
    
    let count = 0;
    // Map sheet records
    const rows = data.records || data || [];
    rows.forEach(r => {
      // spreadsheet row mapping: Phone, Name, Serial, Product, Warranty Status
      let phone = r.Phone || r[0] || '';
      let name = r.Name || r[1] || '';
      let serial = r.Serial || r[2] || '';
      let product = r.Product || r[3] || '';
      let warranty = r.Warranty || r[4] || 'active';
      
      // format numbers to E.164
      phone = String(phone).trim();
      if (phone && !phone.startsWith("+")) {
        if (phone.length === 10) phone = "+91" + phone;
        else if (phone.length === 12 && phone.startsWith("91")) phone = "+" + phone;
      }

      if (serial) {
        stmt.run([phone, name, serial, product, warranty]);
        count++;
      }
    });
    
    stmt.free();
    db.run("COMMIT;");

    showToast("Database Sync", `Syncing database binary to ESP32 SD card (${count} entries)...`, "info");

    const binaryDb = db.export(); // uint8array
    
    // Upload SQLite DB using raw binary POST multipart upload to /upload_file
    const blob = new Blob([binaryDb], { type: "application/octet-stream" });
    const formData = new FormData();
    formData.append("file", blob, "/db/customers.db");

    const uploadRes = await fetch("/upload_file", {
      method: "POST",
      body: formData
    });
    const uploadResult = await uploadRes.json();

    if (uploadResult.success || uploadResult.ok) {
      showToast("Sync Complete", `Offline SQLite database successfully synchronized. Total: ${count} customers.`, "success");
      // Trigger update sync logs or stats
      document.getElementById("sum-sync").textContent = new Date().toLocaleTimeString();
    } else {
      throw new Error(uploadResult.error || "File upload failed");
    }
  } catch (e) {
    console.error("Database sync failed:", e);
    showToast("Sync Failed", e.message, "danger");
  } finally {
    if (btn) btn.disabled = false;
  }
}

// Flow Validator Checks
function validateFlow() {
  const errors = [];
  const warnings = [];
  const nodes = ivrFlow.nodes;

  if (!nodes["start"]) {
    errors.push("Missing 'Start Call' node (ID: start). Flow cannot run.");
  }

  // Trace graph cycles, loop loops, broken handles
  for (const [id, node] of Object.entries(nodes)) {
    // 1. Check broken connections
    if (node.next && !nodes[node.next]) errors.push(`Step '${node.name}' links to non-existent target ID: '${node.next}'`);
    if (node.fail_node && !nodes[node.fail_node]) errors.push(`Step '${node.name}' (Fail) links to non-existent target ID: '${node.fail_node}'`);
    if (node.success_node && !nodes[node.success_node]) errors.push(`Step '${node.name}' (Success) links to non-existent target ID: '${node.success_node}'`);
    if (node.claimed_node && !nodes[node.claimed_node]) errors.push(`Step '${node.name}' (Claimed) links to non-existent target ID: '${node.claimed_node}'`);

    if (node.options) {
      for (const [key, target] of Object.entries(node.options)) {
        if (!nodes[target]) errors.push(`Step '${node.name}' (Key ${key}) links to non-existent target ID: '${target}'`);
      }
    }

    // 2. Check missing voice clips
    if ((node.type === "play_voice" || node.type === "wait_dtmf" || node.type === "warranty_verify" || node.type === "agent_connect") && !node.audio) {
      warnings.push(`Step '${node.name}' does not have any audio clip selected. It will run silently.`);
    } else if (node.audio && !voiceFiles.includes(node.audio)) {
      warnings.push(`Step '${node.name}' references audio file '${node.audio}' which is not currently present on the SD Card.`);
    }

    // 3. Check missing SMS template text
    if (node.type === "send_sms" && (!node.sms_template || node.sms_template.trim() === "")) {
      errors.push(`Step '${node.name}' is an SMS block but has empty template text.`);
    }
  }

  // Render validation results UI
  const alertBox = document.getElementById("validation-alerts-box");
  if (!alertBox) return errors.length === 0;

  alertBox.innerHTML = "";
  if (errors.length === 0 && warnings.length === 0) {
    alertBox.innerHTML = `<div class="alert alert-success py-2 px-3 small mb-0"><i class="fa-solid fa-circle-check me-2"></i> Flow layout validated successfully. Zero errors or warnings found. Ready to deploy!</div>`;
    return true;
  }

  let html = "";
  errors.forEach(err => {
    html += `<div class="alert alert-danger py-2 px-3 small mb-2"><i class="fa-solid fa-triangle-exclamation me-2"></i> <strong>Error:</strong> ${err}</div>`;
  });
  warnings.forEach(warn => {
    html += `<div class="alert alert-warning py-2 px-3 small mb-2"><i class="fa-solid fa-circle-info me-2"></i> <strong>Warning:</strong> ${warn}</div>`;
  });

  alertBox.innerHTML = html;
  return errors.length === 0; // return true if no errors (warnings are fine to compile/deploy)
}

// Deploy visual IVR configuration to ESP32
async function deployIvrFlow() {
  const btn = document.getElementById("btn-deploy-flow");
  if (btn) btn.disabled = true;

  if (!validateFlow()) {
    showToast("Deployment Blocked", "Please resolve all layout errors before compiling.", "danger");
    if (btn) btn.disabled = false;
    return;
  }

  showToast("Deploy Manager", "Compiling flow into runtime configuration JSON...", "info");

  // Compile visual map to clean structure
  const runtimeFlow = {
    nodes: {}
  };

  for (const [id, node] of Object.entries(ivrFlow.nodes)) {
    runtimeFlow.nodes[id] = {
      type: node.type,
      audio: node.audio || "",
      delay_before: node.delay_before || 0,
      delay_after: node.delay_after || 0,
      interruptible: node.interruptible !== false,
      timeout: node.timeout || 5,
      max_attempts: node.max_attempts || 2,
      next: node.next || "",
      options: node.options || {},
      sms_template: node.sms_template || "",
      success_node: node.success_node || "",
      fail_node: node.fail_node || "",
      claimed_node: node.claimed_node || ""
    };
  }

  try {
    showToast("Deploy Manager", "Uploading `/ivr_flow.json` config payload...", "info");
    const blob = new Blob([JSON.stringify(runtimeFlow, null, 2)], { type: "application/json" });
    const formData = new FormData();
    formData.append("file", blob, "/ivr_flow.json");

    const uploadRes = await fetch("/upload_file", {
      method: "POST",
      body: formData
    });
    const result = await uploadRes.json();

    if (result.success || result.ok) {
      showToast("Deploy Manager", "Activating configuration on ESP32...", "info");
      // Trigger hot-reload on ESP32 without resetting the microcontroller!
      const reloadRes = await fetch("/api/reload_ivr", { method: "POST" });
      const reloadResult = await reloadRes.json();
      
      if (reloadResult.success) {
        showToast("Success", "IVR Flow successfully compiled, uploaded, and hot-deployed on ESP32!", "success");
      } else {
        throw new Error("Hot deployment command failed on ESP32");
      }
    } else {
      throw new Error(result.error || "File upload failed");
    }
  } catch (e) {
    console.error("Hot deployment failed:", e);
    showToast("Deploy Failed", e.message, "danger");
  } finally {
    if (btn) btn.disabled = false;
  }
}

// Call Simulator Logic
let simCallActive = false;
let simCurrentNodeId = null;
let simAttemptCount = 0;
let simVariables = {};
let simAudioTimer = null;
let simDelayTimer = null;

function startCallSimulation() {
  const number = document.getElementById("sim-caller-input")?.value.trim() || "+919092610415";
  
  simCallActive = true;
  simCurrentNodeId = "start";
  simAttemptCount = 0;
  simVariables = {
    "Phone": number,
    "Name": "Ravi Kumar", // Mock lookups
    "Product": "V-Varma Pro Smart",
    "Serial": "VV-109284",
    "Warranty": "active"
  };

  document.getElementById("sim-screen-title").textContent = "IN CALL...";
  document.getElementById("sim-screen-subtitle").textContent = number;
  document.getElementById("sim-btn-start").classList.add("d-none");
  document.getElementById("sim-btn-hangup").classList.remove("d-none");
  document.getElementById("sim-log-output").innerHTML = "";

  logSimEvent("Call Connected.");
  executeSimStep();
}

function hangupCallSimulation() {
  clearTimeout(simAudioTimer);
  clearTimeout(simDelayTimer);
  simCallActive = false;
  simCurrentNodeId = null;

  document.getElementById("sim-screen-title").textContent = "IDLE";
  document.getElementById("sim-screen-subtitle").textContent = "Ready to simulate";
  document.getElementById("sim-btn-start").classList.remove("d-none");
  document.getElementById("sim-btn-hangup").classList.add("d-none");
  logSimEvent("Call Terminated.");
  selectNodeVisual(""); // remove green highlight
}

function logSimEvent(msg) {
  const div = document.getElementById("sim-log-output");
  if (!div) return;
  const time = new Date().toLocaleTimeString([], { hour: '2-digit', minute: '2-digit', second: '2-digit' });
  div.innerHTML += `<div class="sim-log-row"><span class="sim-log-time">[${time}]</span> ${msg}</div>`;
  div.scrollTop = div.scrollHeight;
}

function executeSimStep() {
  if (!simCallActive || !simCurrentNodeId) return;

  const node = ivrFlow.nodes[simCurrentNodeId];
  if (!node) {
    logSimEvent(`ERROR: Step '${simCurrentNodeId}' not found. Hanging up.`);
    hangupCallSimulation();
    return;
  }

  selectNodeVisual(simCurrentNodeId);
  logSimEvent(`Running step: <strong>${node.name}</strong> (type: ${node.type})`);

  // 1. Handle Delay Before
  const beforeDelay = node.delay_before || 0;
  if (beforeDelay > 0) {
    logSimEvent(`Waiting delay_before: ${beforeDelay}ms...`);
  }

  simDelayTimer = setTimeout(() => {
    // 2. Play Audio
    if (node.audio) {
      logSimEvent(`Play Audio: <span class="text-primary">${node.audio.substring(node.audio.lastIndexOf('/')+1)}</span>`);
      // Simulate playing (say, 3 seconds)
      simAudioTimer = setTimeout(() => {
        logSimEvent("Audio playback completed.");
        handleStepAction(node);
      }, 3000);
    } else {
      handleStepAction(node);
    }
  }, beforeDelay);
}

function handleStepAction(node) {
  // 3. Handle Delay After
  const afterDelay = node.delay_after || 0;
  
  simDelayTimer = setTimeout(() => {
    // 4. Run logic based on type
    if (node.type === "play_voice" || node.type === "delay") {
      if (node.next) {
        simCurrentNodeId = node.next;
        executeSimStep();
      } else {
        logSimEvent("No next step configured. Hanging up.");
        hangupCallSimulation();
      }
    }
    else if (node.type === "wait_dtmf") {
      logSimEvent("Waiting for DTMF input (timeout 5s)...");
      // Keypad numbers on the simulator phone will trigger DTMF press
    }
    else if (node.type === "send_sms") {
      let parsedSms = node.sms_template || "";
      for (const [k, v] of Object.entries(simVariables)) {
        parsedSms = parsedSms.replace(`{${k}}`, v);
      }
      logSimEvent(`<strong>SMS Transmitted:</strong> "${parsedSms}"`);
      if (node.next) {
        simCurrentNodeId = node.next;
        executeSimStep();
      } else {
        hangupCallSimulation();
      }
    }
    else if (node.type === "customer_verify") {
      logSimEvent(`Searching database for caller: ${simVariables["Phone"]}`);
      // assume true for mock
      logSimEvent("Caller matched in database record. Load details.");
      simCurrentNodeId = node.success_node || node.next;
      executeSimStep();
    }
    else if (node.type === "warranty_verify") {
      logSimEvent("Awaiting warranty number input ending with #...");
    }
    else if (node.type === "complaint_reg") {
      const ticket = "VRM-" + Math.floor(100000 + Math.random() * 900000);
      simVariables["ComplaintNo"] = ticket;
      logSimEvent(`Logged complaint. Generated Ticket ID: <strong class="text-success">${ticket}</strong>`);
      if (node.next) {
        simCurrentNodeId = node.next;
        executeSimStep();
      } else {
        hangupCallSimulation();
      }
    }
    else if (node.type === "installation_reg") {
      const ticket = "INS-" + Math.floor(100000 + Math.random() * 900000);
      simVariables["ComplaintNo"] = ticket;
      logSimEvent(`Booked installation. Generated Ticket ID: <strong class="text-success">${ticket}</strong>`);
      if (node.next) {
        simCurrentNodeId = node.next;
        executeSimStep();
      } else {
        hangupCallSimulation();
      }
    }
    else if (node.type === "agent_connect") {
      logSimEvent(`Dialing agent target: <strong class="text-danger">${simVariables["Phone"]}</strong>`);
      logSimEvent("ECT Call bridging complete. Agent speaking.");
      // end simulator call after agent bridges
      setTimeout(hangupCallSimulation, 4000);
    }
    else if (node.type === "hangup") {
      hangupCallSimulation();
    }
  }, afterDelay);
}

// Press DTMF key in simulator
function pressSimKey(key) {
  if (!simCallActive) return;
  
  logSimEvent(`DTMF Button Pressed: <strong>${key}</strong>`);
  const node = ivrFlow.nodes[simCurrentNodeId];
  if (!node) return;

  if (node.type === "wait_dtmf") {
    clearTimeout(simAudioTimer); // Interrupt audio
    const target = node.options ? node.options[key] : null;
    if (target) {
      simCurrentNodeId = target;
      executeSimStep();
    } else {
      logSimEvent(`Key '${key}' not mapped to any branch path.`);
      if (node.fail_node) {
        simCurrentNodeId = node.fail_node;
        executeSimStep();
      } else {
        hangupCallSimulation();
      }
    }
  }
  else if (node.type === "warranty_verify") {
    clearTimeout(simAudioTimer);
    if (key === "#") {
      logSimEvent("Warranty input completed.");
      simVariables["Warranty"] = "active"; // mock valid
      simCurrentNodeId = node.success_node;
      executeSimStep();
    }
  }
}

// Stop all playing preview audios and reset their button state
function stopAllPreviewAudios() {
  const allAudios = document.querySelectorAll('audio[id^="audio-preview-"]');
  allAudios.forEach(aud => {
    if (!aud.paused) {
      aud.pause();
      aud.currentTime = 0;
      const otherIdx = aud.id.split('-').pop();
      const otherBtn = document.getElementById(`btn-preview-${otherIdx}`);
      if (otherBtn) {
        otherBtn.innerHTML = '<i class="fa-solid fa-play"></i> Play';
        otherBtn.classList.remove('btn-danger');
        otherBtn.classList.add('btn-glass-primary');
      }
    }
  });
}

// Toggle audio playback with Play/Stop state using fetch+blob to avoid CORS/autoplay blocks
function togglePreviewAudio(idx) {
  const btn = document.getElementById(`btn-preview-${idx}`);
  const audio = document.getElementById(`audio-preview-${idx}`);
  if (!btn || !audio) return;

  // If currently playing this track — STOP it
  if (!audio.paused) {
    audio.pause();
    audio.currentTime = 0;
    btn.innerHTML = '<i class="fa-solid fa-play"></i> Play';
    btn.classList.remove('btn-danger');
    btn.classList.add('btn-glass-primary');
    return;
  }

  // Pause all other audios first
  stopAllPreviewAudios();

  // If a blob URL is already loaded, play directly
  if (audio.src && audio.src.startsWith('blob:')) {
    audio.play().then(() => {
      btn.innerHTML = '<i class="fa-solid fa-stop"></i> Stop';
      btn.classList.remove('btn-glass-primary');
      btn.classList.add('btn-danger');
    }).catch(err => {
      console.error('Audio play failed:', err);
      if (typeof showToast === 'function') showToast('Playback Error', 'Browser blocked audio. Try clicking the page first.', 'danger');
    });
    audio.onended = () => {
      btn.innerHTML = '<i class="fa-solid fa-play"></i> Play';
      btn.classList.remove('btn-danger');
      btn.classList.add('btn-glass-primary');
    };
    return;
  }

  // Fetch audio as blob to bypass CORS/autoplay policies on plain HTTP
  const filePath = audio.dataset.src;
  btn.innerHTML = '<i class="fa-solid fa-spinner fa-spin"></i> Loading';
  btn.disabled = true;

  fetch(filePath)
    .then(res => {
      if (!res.ok) throw new Error(`HTTP ${res.status}`);
      return res.blob();
    })
    .then(blob => {
      const blobUrl = URL.createObjectURL(blob);
      audio.src = blobUrl;
      return audio.play();
    })
    .then(() => {
      btn.disabled = false;
      btn.innerHTML = '<i class="fa-solid fa-stop"></i> Stop';
      btn.classList.remove('btn-glass-primary');
      btn.classList.add('btn-danger');
      audio.onended = () => {
        btn.innerHTML = '<i class="fa-solid fa-play"></i> Play';
        btn.classList.remove('btn-danger');
        btn.classList.add('btn-glass-primary');
      };
    })
    .catch(err => {
      btn.disabled = false;
      btn.innerHTML = '<i class="fa-solid fa-play"></i> Play';
      console.error('Audio fetch/play failed:', err);
      if (typeof showToast === 'function') showToast('Playback Error', 'Could not load audio: ' + err.message, 'danger');
    });
}

// UI Rendering elements for menus
function renderVoiceLibraryUI(files) {
  const container = document.getElementById("voice-library-table-body");
  if (!container) return;

  container.innerHTML = "";
  const wavFiles = files.filter(f => f.name.toLowerCase().endsWith(".wav") || f.name.toLowerCase().endsWith(".mp3"));

  if (wavFiles.length === 0) {
    container.innerHTML = `<tr><td colspan="5" class="text-muted text-center py-4">No audio files found on SD card directory</td></tr>`;
    return;
  }

  wavFiles.forEach((file, idx) => {
    const tr = document.createElement("tr");
    tr.innerHTML = `
      <td>${idx + 1}</td>
      <td class="fw-semibold text-white">${file.name}</td>
      <td>${Math.round(file.size / 1024)} KB</td>
      <td>
        <audio id="audio-preview-${idx}" data-src="/download_file?file=${encodeURIComponent(file.name)}"></audio>
        <button id="btn-preview-${idx}" class="btn btn-glass-primary btn-xs py-1" onclick="togglePreviewAudio(${idx})"><i class="fa-solid fa-play"></i> Play</button>
      </td>
      <td>
        <button class="btn btn-link text-danger p-0 ms-2" onclick="deleteAudioFile('${file.name}')"><i class="fa-solid fa-trash-can"></i></button>
      </td>
    `;
    container.appendChild(tr);
  });
}

async function deleteAudioFile(path) {
  if (!confirm(`Are you sure you want to delete ${path}?`)) return;
  try {
    const formData = new FormData();
    formData.append("file", path);
    const res = await fetch("/delete_audio", {
      method: "POST",
      body: formData
    });
    const result = await res.json();
    if (result.success) {
      showToast("Success", "Audio file removed successfully", "success");
      scanVoiceLibrary();
    } else {
      throw new Error(result.error || "Delete failed");
    }
  } catch (e) {
    showToast("Delete Failed", e.message, "danger");
  }
}

/**
 * handleVoiceUpload()
 * Called by the Voice Manager upload form (onsubmit="handleVoiceUpload()").
 * Reads the selected local audio file and uploads it to the ESP32 SD card
 * at the user-specified destination path via POST /upload_file.
 */
async function handleVoiceUpload() {
  const destEl = document.getElementById("upload-sd-dest");
  const fileEl = document.getElementById("upload-audio-file");
  const btn    = document.getElementById("btn-upload-audio");

  if (!destEl || !fileEl || !fileEl.files.length) {
    showToast("Upload Error", "Please select a file and specify the target SD path.", "warning");
    return;
  }

  const dest = destEl.value.trim();
  if (!dest) {
    showToast("Upload Error", "Target SD path cannot be empty (e.g. /01/018.wav).", "warning");
    return;
  }
  if (!dest.startsWith("/")) {
    showToast("Upload Error", "Path must start with '/' (e.g. /01/018.wav).", "warning");
    return;
  }

  const file = fileEl.files[0];
  const ext  = file.name.split('.').pop().toLowerCase();
  if (!['wav', 'mp3'].includes(ext)) {
    showToast("Upload Error", "Only .wav and .mp3 files are supported.", "warning");
    return;
  }

  if (btn) { btn.disabled = true; btn.innerHTML = '<i class="fa-solid fa-spinner fa-spin me-2"></i> Uploading…'; }
  showToast("Upload", `Uploading ${file.name} → ${dest}`, "info");

  try {
    const formData = new FormData();
    formData.append("file", file, dest);
    formData.append("path", dest);

    const res = await fetch("/upload_file", { method: "POST", body: formData });
    let result;
    try { result = await res.json(); } catch (_) { result = { success: res.ok }; }

    if (result.success || result.ok || res.ok) {
      showToast("Upload Complete", `${file.name} uploaded to ${dest} successfully.`, "success");
      destEl.value = "";
      fileEl.value = "";
      setTimeout(() => scanVoiceLibrary(), 600);
    } else {
      throw new Error(result.error || "Upload failed — ESP32 returned error.");
    }
  } catch (e) {
    console.error("[VoiceUpload] Error:", e);
    showToast("Upload Failed", e.message, "danger");
  } finally {
    if (btn) { btn.disabled = false; btn.innerHTML = '<i class="fa-solid fa-cloud-arrow-up me-2"></i> Upload to SD Card'; }
  }
}

// ============================================================
// SMS TEMPLATES — Full CRUD with ESP32 persistence
// ============================================================

/**
 * Load SMS templates:
 *  1. Try GET /api/sms_templates from ESP32/SD
 *  2. On failure, fall back to built-in defaults
 *  3. Render the full CRUD UI
 */
async function loadSmsTemplates() {
  const defaults = [
    { name: "Complaint Registered", active: true, body: "Dear {Name}, your complaint has been registered. Complaint No: {ComplaintNo}. Date: {CurrentDate}. V-Varma." },
    { name: "Warranty Valid",       active: true, body: "Dear {Name}, your product warranty is VALID. Product: {Product}, Serial: {Serial}. Warranty Expires: {Warranty}." },
    { name: "Installation Booked", active: true, body: "Dear {Name}, your installation is scheduled. Product: {Product}. Our team will contact you shortly." },
    { name: "Missed Call Alert",   active: true, body: "Dear customer, we missed your call. We will get back to you shortly. Website: vvarma.gsvee.in" }
  ];

  try {
    const res = await fetch('/api/sms_templates');
    if (res.ok) {
      const data = await res.json();
      if (Array.isArray(data) && data.length > 0) {
        smsTemplates = data;
        console.log('[SMS Templates] Loaded', smsTemplates.length, 'templates from ESP32 SD card.');
      } else {
        smsTemplates = defaults;
        console.warn('[SMS Templates] ESP32 returned empty list. Using defaults.');
      }
    } else {
      throw new Error('HTTP ' + res.status);
    }
  } catch (e) {
    console.warn('[SMS Templates] Failed to load from ESP32 (' + e.message + '). Using defaults.');
    smsTemplates = defaults;
  }

  renderSmsTemplatesUI();
}

/**
 * Save all smsTemplates to ESP32 SD card via POST /api/sms_templates
 */
async function saveSmsTemplateToESP32() {
  try {
    const res = await fetch('/api/sms_templates', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify(smsTemplates)
    });
    const result = await res.json();
    if (result.success) {
      showToast('Templates Saved', 'SMS templates written to SD card successfully.', 'success');
    } else {
      throw new Error(result.error || 'Save failed');
    }
  } catch (e) {
    console.warn('[SMS Templates] Save to ESP32 failed:', e.message);
    showToast('Save Warning', 'Templates saved locally only. ESP32 may be offline.', 'warning');
  }
}

/**
 * Render the SMS Templates page with full Add / Edit / Delete CRUD controls.
 */
function renderSmsTemplatesUI() {
  const container = document.getElementById("sms-templates-list-container");
  if (!container) return;

  // Build header toolbar
  let html = `
  <div class="d-flex justify-content-between align-items-center mb-3">
    <span class="text-muted small"><i class="fa-solid fa-circle-info text-info me-1"></i>${smsTemplates.length} template(s) configured</span>
    <button class="btn btn-glass-primary btn-sm" onclick="addSmsTemplate()">
      <i class="fa-solid fa-plus me-1"></i> New Template
    </button>
  </div>`;

  if (smsTemplates.length === 0) {
    html += `<div class="glass-card text-center py-5">
      <i class="fa-solid fa-comment-slash fa-2x text-muted mb-3"></i>
      <p class="text-muted small">No templates yet. Click <strong>New Template</strong> to create one.</p>
    </div>`;
  } else {
    html += smsTemplates.map((t, idx) => `
    <div class="glass-card mb-3 p-3" id="sms-tpl-card-${idx}">
      <div class="d-flex justify-content-between align-items-center mb-2">
        <div class="d-flex align-items-center gap-2">
          <i class="fa-solid fa-comment-sms ${t.active ? 'text-success' : 'text-secondary'}"></i>
          <span class="fw-bold text-white small">${escapeHtml(t.name)}</span>
          <span class="badge ${t.active ? 'bg-success' : 'bg-secondary'} px-2 py-1" style="font-size:0.68rem;">
            ${t.active ? 'Active' : 'Inactive'}
          </span>
        </div>
        <div class="d-flex gap-1">
          <button class="btn btn-xs btn-glass-secondary py-1 px-2" title="Toggle Active" onclick="toggleSmsTemplate(${idx})">
            <i class="fa-solid fa-${t.active ? 'toggle-on text-success' : 'toggle-off text-muted'}"></i>
          </button>
          <button class="btn btn-xs btn-glass-secondary py-1 px-2" title="Edit Template" onclick="editSmsTemplate(${idx})">
            <i class="fa-solid fa-pen-to-square text-warning"></i>
          </button>
          <button class="btn btn-xs btn-glass-danger py-1 px-2" title="Delete Template" onclick="deleteSmsTemplate(${idx})">
            <i class="fa-solid fa-trash-can"></i>
          </button>
        </div>
      </div>
      <p class="text-muted small m-0 font-monospace" style="font-size:0.78rem; white-space:pre-wrap;">${escapeHtml(t.body)}</p>
      <div class="mt-2 pt-2 border-top border-secondary" id="sms-edit-section-${idx}" style="display:none;">
        <label class="form-label text-muted" style="font-size:0.72rem;">Template Name</label>
        <input type="text" id="sms-edit-name-${idx}" class="form-control glass-input mb-2" style="font-size:0.82rem;" value="${escapeHtml(t.name)}">
        <label class="form-label text-muted" style="font-size:0.72rem;">Message Body</label>
        <textarea id="sms-edit-body-${idx}" class="form-control glass-input mb-2" rows="4" style="font-size:0.82rem;">${escapeHtml(t.body)}</textarea>
        <div class="mb-2">
          <span class="text-muted" style="font-size:0.72rem;">Insert variable: </span>
          ${['{Name}','{Phone}','{Product}','{Serial}','{ComplaintNo}','{Warranty}','{CurrentDate}'].map(v =>
            `<button class="btn btn-xs btn-glass-secondary me-1 mb-1 py-0 px-1" style="font-size:0.7rem;" onclick="insertSmsVar(${idx},'${v}')">${v}</button>`
          ).join('')}
        </div>
        <div class="d-flex gap-2">
          <button class="btn btn-glass-primary btn-sm flex-fill" onclick="saveSmsTemplateEdit(${idx})"><i class="fa-solid fa-floppy-disk me-1"></i> Save</button>
          <button class="btn btn-glass-secondary btn-sm" onclick="cancelSmsTemplateEdit(${idx})">Cancel</button>
        </div>
      </div>
    </div>`);
  }

  container.innerHTML = html;
}

/** Toggle active/inactive state of a template */
function toggleSmsTemplate(idx) {
  if (idx < 0 || idx >= smsTemplates.length) return;
  smsTemplates[idx].active = !smsTemplates[idx].active;
  saveSmsTemplateToESP32();
  renderSmsTemplatesUI();
}

/** Show the inline edit section for a template card */
function editSmsTemplate(idx) {
  // Hide any other open editors first
  document.querySelectorAll('[id^="sms-edit-section-"]').forEach(el => el.style.display = 'none');
  const section = document.getElementById(`sms-edit-section-${idx}`);
  if (section) section.style.display = 'block';
}

/** Cancel editing */
function cancelSmsTemplateEdit(idx) {
  const section = document.getElementById(`sms-edit-section-${idx}`);
  if (section) section.style.display = 'none';
}

/** Save edited template values */
function saveSmsTemplateEdit(idx) {
  const nameEl = document.getElementById(`sms-edit-name-${idx}`);
  const bodyEl = document.getElementById(`sms-edit-body-${idx}`);
  if (!nameEl || !bodyEl) return;
  const name = nameEl.value.trim();
  const body = bodyEl.value.trim();
  if (!name || !body) {
    showToast('Validation', 'Template name and body cannot be empty.', 'warning');
    return;
  }
  smsTemplates[idx].name = name;
  smsTemplates[idx].body = body;
  saveSmsTemplateToESP32();
  renderSmsTemplatesUI();
  showToast('Saved', `Template "${name}" updated successfully.`, 'success');
}

/** Insert a variable placeholder at the cursor position in the textarea */
function insertSmsVar(idx, variable) {
  const bodyEl = document.getElementById(`sms-edit-body-${idx}`);
  if (!bodyEl) return;
  const start = bodyEl.selectionStart;
  const end = bodyEl.selectionEnd;
  const text = bodyEl.value;
  bodyEl.value = text.substring(0, start) + variable + text.substring(end);
  bodyEl.selectionStart = bodyEl.selectionEnd = start + variable.length;
  bodyEl.focus();
}

/** Delete a template at the given index */
function deleteSmsTemplate(idx) {
  if (!confirm(`Delete template "${smsTemplates[idx]?.name}"? This cannot be undone.`)) return;
  smsTemplates.splice(idx, 1);
  saveSmsTemplateToESP32();
  renderSmsTemplatesUI();
  showToast('Deleted', 'SMS template removed.', 'success');
}

/** Add a new blank template and open its edit section */
function addSmsTemplate() {
  smsTemplates.push({ name: 'New Template', active: true, body: 'Dear {Name}, ' });
  renderSmsTemplatesUI();
  // Auto-open the last item for editing
  const lastIdx = smsTemplates.length - 1;
  editSmsTemplate(lastIdx);
  const card = document.getElementById(`sms-tpl-card-${lastIdx}`);
  if (card) card.scrollIntoView({ behavior: 'smooth', block: 'start' });
}

/** Escape HTML for safe rendering */
function escapeHtml(str) {
  if (!str) return '';
  return String(str)
    .replace(/&/g, '&amp;')
    .replace(/</g, '&lt;')
    .replace(/>/g, '&gt;')
    .replace(/"/g, '&quot;')
    .replace(/'/g, '&#39;');
}

// Custom tabs navigation listener
document.addEventListener("DOMContentLoaded", () => {
  document.querySelectorAll(".sidebar-link").forEach(link => {
    link.addEventListener("click", (e) => {
      const target = link.dataset.target;
      if (target === "ivr-flow-builder") {
        setTimeout(redrawConnections, 100); // redraw canvas SVG connections
      }
    });
  });
});

function updateDeleteButtonState() {
  const btnDelete = document.getElementById("btn-tool-delete");
  if (btnDelete) {
    if (selectedConnection || currentSelectedNode) {
      btnDelete.removeAttribute("disabled");
    } else {
      btnDelete.setAttribute("disabled", "true");
    }
  }
}

function deleteSelectedConnection() {
  if (!selectedConnection) return;
  const { fromId, toId, label } = selectedConnection;
  const node = ivrFlow.nodes[fromId];
  if (node) {
    if (node.next === toId && label === "Next") {
      node.next = "";
    } else if (node.fail_node === toId && label === "Fail / Timeout") {
      node.fail_node = "";
    } else if (node.success_node === toId && label === "Success") {
      node.success_node = "";
    } else if (node.claimed_node === toId && label === "Claimed") {
      node.claimed_node = "";
    } else if (node.options) {
      for (const [key, val] of Object.entries(node.options)) {
        if (val === toId && `Key ${key}` === label) {
          delete node.options[key];
        }
      }
    }
    showToast("Success", "Connection deleted", "success");
    selectedConnection = null;
    updateDeleteButtonState();
    saveAndRedraw();
  }
}

function handleDeleteSelection() {
  if (selectedConnection) {
    deleteSelectedConnection();
  } else if (currentSelectedNode) {
    if (currentSelectedNode.id === "start") {
      showToast("Error", "Start node cannot be deleted", "danger");
      return;
    }
    if (confirm(`Are you sure you want to delete the step "${currentSelectedNode.name}"?`)) {
      deleteNode(currentSelectedNode.id);
      currentSelectedNode = null;
      updateDeleteButtonState();
    }
  } else {
    showToast("Delete", "Please select a step or a connection line first.", "warning");
  }
}
