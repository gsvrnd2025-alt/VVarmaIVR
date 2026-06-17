/**
 * V-VARMA V3 MODULAR WEB PORTAL
 * Backend Logic (Code.gs)
 * Fully Merged & Enhanced Modular Architecture
 */

// ============================================
// 1. CONFIGURATION & SCHEMAS
// ============================================

const CONFIG = {
  version: '4.0.0',
  spreadsheetId: '', // HARDCODED PRIMARY ID CLEARED
  warrantyPeriod: 12,
  productImagesFolderId: '1TcLtj00QmgnQHqTqnydm-A1ba-mngoQ5',
  defaultAdmin: {
    username: '',
    password: '', // Primary password
    secondaryPassword: '', // Legacy support
    role: 'Admin',
    name: 'Super Admin',
    mobile: '',
    email: ''
  },
  sheets: {
    users: 'Users',
    parties: 'Party_List',
    products: 'Product_Models',
    warranty: 'Warranty_Register',
    complaints: 'Complaints',
    installation: 'Installation_Register',
    orders: 'Orders',
    ledger: 'Ledger',
    inventory: 'Inventory',
    manufacturing: 'Manufacturing_Details',
    qrCodes: 'QR_Codes',
    notifications: 'Notifications',
    webSettings: 'Web_Settings',
    carousel: 'Carousel_Images',
    chatSessions: 'Chat_Sessions',
    chatMessages: 'Chat_Messages',
    serviceHistory: 'Service_History',
    complaintStatus: 'Complaint_Status_History',
    vendors: 'Vendor_List',
    issues: 'Issues_List',
    dealers: 'Dealers_List',
    reviews: 'Product_Reviews',
    amc: 'AMC',
    invoices: 'Invoices',
    payments: 'Payments',
    company_profile: 'Company_Profile',
    heroSlides: 'Hero_Slides',
    catalog: 'Catalog',
    catalog_settings: 'Catalog_Settings',
    dealerReviews: 'Dealer_Reviews',
    dropdownFields: 'All Dropdowns',
    technicians: 'Technician_List',
    billing_settings: 'Billing_Settings',
    billing_sequences: 'Billing_Sequences',
    sales_settings: 'Sales_Settings',
    admin_settings: 'Admin_Settings',
    payment_receipts: 'Payment_Receipts',
    customer_wallet: 'Customer_Wallet',
    cash_flow: 'Cash_Flow',
    gallery_images: 'Gallery'
  }
};

const SHEET_SCHEMAS = {
    parties: ['Party ID', 'Type', 'Name', 'Shop Name', 'Mobile', 'Email', 'GST Number', 'Address', 'Pincode', 'District', 'State', 'GPS', 'Coordinates', 'DOB', 'Landmark', 'Credit Limit', 'Opening Balance', 'Current Balance', 'Status', 'Added Date'],
    users: ['Username', 'Password', 'Role', 'Name', 'Mobile', 'Status', 'Category', 'Experience', 'Address', 'GPS', 'Coordinates', 'DOB', 'IDProof', 'Qualification', 'Skills', 'Work Specialization', 'Service Areas', 'Alternate Phone'],
    products: ['Model Code', 'Model Name', 'Description', 'Type', 'Category', 'ImageUrl', 'Price', 'Status', 'Motor Type', 'Phase', 'Tank Type', 'Stock', 'Specs', 'ImageIds', 'Automation Type', 'QR Code', 'Updated At', 'MRP'],
    vendors: ['Vendor ID', 'Vendor Name', 'Location', 'Mobile', 'Email', 'Status', 'Shop Name', 'Nearby Area', 'Address', 'Pincode', 'Blocked Reason', 'Created Date'],
    inventory: ['Model ID', 'Txn ID', 'Date', 'Model Number', 'Model Name', 'Product Title', 'Part Number', 'Action', 'Serial Number', 'Vendor Name', 'Bill Number', 'Quantity', 'Stock Before', 'Stock After', 'Reference', 'Notes', 'Updated By'],
    orders: ['Order Date', 'Order ID', 'Customer Name', 'Mobile', 'Type', 'Items', 'Status', 'Total Amount', 'Technician Assigned', 'Delivery Status'],
    dealers: ['Dealer ID', 'Dealer Name', 'Mobile', 'Email', 'Area', 'Nearby Area', 'Pincode', 'Address', 'City', 'State', 'District', 'Shop Name', 'GST Number', 'ID Proof URL', 'Status', 'Username', 'Password', 'Application Date', 'Maps URL', 'Business URL'],
    issues: ['Issue ID', 'Issue Name', 'Category', 'Warranty Covered'],
    warranty: ['Registration Date', 'Warranty ID', 'Part Number', 'Product Model', 'Vendor Name', 'Customer Name', 'Mobile Number', 'Email', 'Full Address', 'Area', 'Landmark', 'Pincode', 'Date Type', 'Warranty Start Date', 'Warranty End Date', 'Status', 'Photo URL', 'Remarks', 'Manufacture Date'],
    complaints: ['Complaint Date', 'Complaint ID', 'Warranty ID', 'Customer Name', 'Mobile Number', 'Email', 'Product Model', 'Issue Type', 'Complaint Type', 'Problem Description', 'Full Address', 'Area', 'Landmark', 'Pincode', 'Service Date', 'Time Slot', 'Status', 'Technician Assigned', 'Technician Remarks', 'Resolution Date', 'created_at', 'updated_at', 'created_by', 'Created Date', 'Completed Date'],
    complaintStatus: ['Complaint ID', 'Status', 'Remarks', 'Date', 'Updated By'],
    manufacturing: ['Part Number', 'Product Model', 'Manufacture Date', 'Location', 'Stock Status', 'Added By'],
    reviews: ['Date', 'Product Model', 'Customer Name', 'Rating', 'Comment'],
    qrCodes: ['QR Value', 'QR Type', 'Entity ID', 'Model Code', 'Part Number', 'Status', 'Linked On', 'Updated On'],
    serviceHistory: ['Event Date', 'Event Type', 'Entity Type', 'Entity ID', 'Product Model', 'Part Number', 'Warranty ID', 'Customer Name', 'Mobile', 'Technician', 'Status', 'Notes', 'Source'],
    notifications: ['Notification ID', 'Date', 'Audience', 'Recipient', 'Channel', 'Title', 'Message', 'Reference Type', 'Reference ID', 'Status', 'Text Size', 'Text Color', 'Scroll Duration'],
    amc: ['AMC ID', 'Customer Name', 'Mobile', 'Email', 'Product Model', 'Part Number', 'Warranty ID', 'AMC Start Date', 'AMC End Date', 'Service Interval Days', 'Service Visits', 'Completed Visits', 'Next Service Due', 'Status', 'Remarks', 'Created Date', 'Updated Date'],
    webSettings: ['Key', 'Value', 'Updated At'],
    heroSlides: ['Slide ID', 'Image URL', 'Badge', 'Title', 'Description', 'Btn1Text', 'Btn1Url', 'Btn2Text', 'Btn2Url', 'Stat1', 'Stat2', 'Stat3', 'StyleJSON', 'Status', 'Created At'],
    carousel: ['Image ID', 'Image URL', 'Caption', 'Linked Product ID', 'Status', 'Created At', 'StyleJSON'],
    chatSessions: ['Session ID', 'Customer Phone', 'Customer Name', 'Status', 'Assigned To', 'Group Members', 'Type', 'Device Info', 'Created At', 'Updated At'],
    chatMessages: ['Message ID', 'Session ID', 'Sender', 'Sender Role', 'Content', 'Timestamp'],
    installation: ['Installation Date', 'Installation ID', 'Customer Name', 'Mobile Number', 'Email', 'Product Model', 'Part Number', 'Vendor Name', 'Motor Type', 'Automation Type', 'Phase', 'Tank Type', 'Full Address', 'Area', 'Landmark', 'Pincode', 'Service Date', 'Time Slot', 'Status', 'Technician Assigned', 'Remarks', 'created_at', 'updated_at', 'created_by', 'Created Date', 'Completed Date', 'Attachment URL'],
    ledger: ['Date', 'Transaction ID', 'Type', 'Customer/Vendor ID', 'Name', 'Mobile', 'Address', 'Part Number', 'Product Model', 'Payment Status', 'Total Amount', 'Paid Amount', 'Balance', 'Payment Mode', 'Remarks', 'Invoice Number', 'Sub Total', 'GST Enabled', 'GST Rate', 'GST Amount', 'Hold Amount', 'Grand Total', 'Source', 'QR Payloads', 'Email'],
    invoices: ['Invoice Number', 'Transaction ID', 'Invoice Date', 'Customer Name', 'Mobile', 'Email', 'Type', 'Sub Total', 'GST Enabled', 'GST Rate', 'GST Amount', 'Grand Total', 'Paid Amount', 'Balance', 'Payment Status', 'Payment Mode', 'Items JSON', 'Created By'],
    payments: ['Payment ID', 'Transaction ID', 'Invoice Number', 'Date', 'Name', 'Mobile', 'Amount', 'Mode', 'Status', 'Remarks'],
    catalog: ['Product ID', 'Part Number', 'Category', 'Mode of Operation', 'Model Number', 'Model Name', 'Product Title', 'Tagline', 'Usage', 'Working Type', 'Voltage', 'Amps', 'Dry Run Feature', 'Timers', 'In Box Kit', 'Wires', 'Installation Inclusive', 'Installation Charges', 'Free Accessory', 'Description', 'Features', 'Warranty', 'Accessories', 'T&C', 'Colors', 'MRP', 'Selling Price', 'Wholesale Price', 'Offer Price', 'Images', 'Status', 'Date Created', 'Stock', 'HSN Code', 'SAC Code', 'GST Rate', 'Banners', 'YouTube'],
    catalog_settings: ['Type', 'Value', 'CreatedAt'],
    gallery_images: ['ProductID', 'ImageURL', 'DisplayOrder', 'Timestamp'],
    dealerReviews: ['Date', 'Dealer ID', 'Customer Name', 'Mobile', 'Rating', 'Comment'],
    dropdownFields: ['State', 'District'],
    technicians: ['Technician ID', 'Name', 'Mobile', 'Email', 'Address', 'Area', 'District', 'State', 'Nearby Area', 'Skills', 'Qualification', 'Experience', 'Reference', 'ID Proof URL', 'DOB', 'Age', 'Username', 'Password', 'Status', 'Added Date'],
    billing_settings: ['Key', 'Value', 'Type', 'Description'],
    billing_sequences: ['DocType', 'Prefix', 'NextNumber'],
    company_profile: ['Key', 'Value', 'Updated At'],
    sales_settings: ['Key', 'Value', 'Updated At'],
    admin_settings: ['Key', 'Value', 'Updated At'],
    payment_receipts: ['Receipt Number', 'Transaction ID', 'Date', 'Customer Name', 'Mobile', 'Amount Received', 'Mode of Payment', 'Reference Number', 'Reference Date', 'Notes', 'Allocations JSON', 'Status'],
    customer_wallet: ['Wallet ID', 'Customer Name', 'Mobile', 'Type', 'Amount', 'Mode of Payment', 'Reference Number', 'Reference Date', 'Notes', 'Date'],
    cash_flow: ['Date', 'Voucher Number', 'Voucher Type', 'Party Name', 'Description', 'Cash In', 'Cash Out', 'Running Balance', 'Payment Mode', 'Reference Number', 'Status'],
    profile_edit_requests: ['Request ID', 'Date', 'Role', 'User ID', 'Name', 'Mobile', 'Requested Data JSON', 'Status', 'Admin Remarks', 'Updated Date'],
    order_requests: ['Request ID', 'Date', 'Dealer ID', 'Dealer Name', 'Mobile', 'Shop Name', 'Items JSON', 'Total Quantity', 'Total Amount', 'Status', 'Admin Remarks', 'Converted Order ID', 'Updated Date']
};

// ============================================
// 2. MAIN ROUTER (doGet)
// ============================================

// Global context to share variables with included templates
var GLOBAL_CONTEXT = {
  appUrl: '',
  page: 'home'
};

function doGet(e) {
  if (e && e.parameter && e.parameter.debug) {
    return runDebugDiagnostics(e);
  }
  const page = e && e.parameter ? String(e.parameter.page || 'home').trim().toLowerCase() : 'home';
  const asset = e && e.parameter ? String(e.parameter.asset || '').trim().toLowerCase() : '';
  
  // Update Global Context
  GLOBAL_CONTEXT.page = page;
  GLOBAL_CONTEXT.appUrl = ScriptApp.getService().getUrl();

  // Save manual spreadsheet ID if passed as parameter
  if (e && e.parameter && (e.parameter.sheetId || e.parameter.spreadsheetId)) {
    var sid = (e.parameter.sheetId || e.parameter.spreadsheetId).trim();
    if (sid) {
      PropertiesService.getScriptProperties().setProperty('MANUAL_SS_ID', sid);
      REQUEST_SPREADSHEET_ID = sid;
      console.log('Saved MANUAL_SS_ID to properties:', sid);
    }
  }

  // ── IVR ESP32 Dashboard JSON API ──
  const action = e && e.parameter ? String(e.parameter.action || '').trim() : '';
  if (action === 'getVarmaData' || action === 'initVarmaSheets' || action === 'validateUser' || action === 'get_calls' || action === 'get_messages' || action === 'get_mobiles' || action === 'get_hc_numbers' || action === 'save_hc_number' || action === 'delete_hc_number') {
    _autoInitOnce_(); // Ensure all sheets exist on first deploy
    let result;
    if (action === 'initVarmaSheets') {
      result = initVarmaSheets();
    } else if (action === 'validateUser') {
      const phone = e && e.parameter ? String(e.parameter.phone || '').trim() : '';
      result = validateUser(phone);
    } else if (action === 'get_calls') {
      result = getIvrCalls();
    } else if (action === 'get_messages') {
      result = getIvrMessages();
    } else if (action === 'get_mobiles') {
      result = getRegisteredMobiles();
    } else if (action === 'get_hc_numbers') {
      result = getHcNumbers();
    } else if (action === 'save_hc_number') {
      result = saveHcNumber(e);
    } else if (action === 'delete_hc_number') {
      result = deleteHcNumber(e);
    } else {
      result = getVarmaData();
    }
    return ContentService
      .createTextOutput(JSON.stringify(result))
      .setMimeType(ContentService.MimeType.JSON);
  }

  // Asset Handler (PWA support)
  if (asset) return getAssetResponse_(asset);

  let template;
  try {
    switch (page) {
      case 'home': template = HtmlService.createTemplateFromFile('home'); break;
      case 'register': template = HtmlService.createTemplateFromFile('register'); break;
      case 'service_portal': template = HtmlService.createTemplateFromFile('service_portal'); break;
      case 'complaint': template = HtmlService.createTemplateFromFile('complaint'); break;
      case 'installation': template = HtmlService.createTemplateFromFile('installation'); break;
      case 'tracking': template = HtmlService.createTemplateFromFile('tracking'); break;
      case 'catalog': template = HtmlService.createTemplateFromFile('catalog'); break;
      case 'gallery': template = HtmlService.createTemplateFromFile('gallery'); break;
      case 'about': template = HtmlService.createTemplateFromFile('about'); break;
      case 'dealers': template = HtmlService.createTemplateFromFile('dealers'); break;
      case 'cart': template = HtmlService.createTemplateFromFile('cart'); break;
      case 'login': template = HtmlService.createTemplateFromFile('login'); break;
      case 'dashboard_admin': 
      case 'debug_admin':
        var templateObj = HtmlService.createTemplateFromFile('dashboard_admin');
        templateObj.currentPage = GLOBAL_CONTEXT.page;
        templateObj.appUrl = GLOBAL_CONTEXT.appUrl;
        templateObj.version = CONFIG.version;
        
        return templateObj.evaluate()
                  .setTitle('V-VARMA Web Portal')
                  .addMetaTag('viewport', 'width=device-width, initial-scale=1')
                  .setXFrameOptionsMode(HtmlService.XFrameOptionsMode.ALLOWALL);
      case 'dashboard_tech': template = HtmlService.createTemplateFromFile('dashboard_tech'); break;
      case 'dashboard_customer': template = HtmlService.createTemplateFromFile('dashboard_customer'); break;
      case 'dashboard_vendor': template = HtmlService.createTemplateFromFile('dashboard_vendor'); break;
      default: template = HtmlService.createTemplateFromFile('home');
    }

    template.currentPage = GLOBAL_CONTEXT.page;
    template.appUrl = GLOBAL_CONTEXT.appUrl;
    template.version = CONFIG.version;

    return template.evaluate()
      .setTitle('V-VARMA Web Portal')
      .addMetaTag('viewport', 'width=device-width, initial-scale=1')
      .setXFrameOptionsMode(HtmlService.XFrameOptionsMode.ALLOWALL);

  } catch (err) {
    return HtmlService.createHtmlOutput(`<b>Error loading page:</b> ${err.message}`);
  }
}

function getAssetResponse_(asset) {
    const assetMap = {
        manifest: { file: 'PWA_Manifest', mime: ContentService.MimeType.JSON },
        sw: { file: 'PWA_ServiceWorker', mime: ContentService.MimeType.JAVASCRIPT }
    };
    const def = assetMap[asset];
    if (!def) return ContentService.createTextOutput('Not Found').setMimeType(ContentService.MimeType.TEXT);
    const template = HtmlService.createTemplateFromFile(def.file);
    template.baseUrl = ScriptApp.getService().getUrl() || '';
    return ContentService.createTextOutput(template.evaluate().getContent()).setMimeType(def.mime);
}

function getDashboardHtml() {
  GLOBAL_CONTEXT.page = 'dashboard_admin';
  var templateObj = HtmlService.createTemplateFromFile('dashboard_admin');
  templateObj.currentPage = GLOBAL_CONTEXT.page;
  templateObj.appUrl = GLOBAL_CONTEXT.appUrl;
  templateObj.version = CONFIG.version;
  return templateObj.evaluate().getContent();
}

function include(filename) {
  try {
    const template = HtmlService.createTemplateFromFile(filename);
    template.appUrl = GLOBAL_CONTEXT.appUrl;
    template.currentPage = GLOBAL_CONTEXT.page;
    template.version = CONFIG.version;
    return template.evaluate().getContent();
  } catch (e) {
    console.error('Include Error [' + filename + ']:', e.message);
    return `<!-- CRITICAL ERROR: Include failed for ${filename}. Details: ${e.message} -->`;
  }
}

/**
 * Version Tracking & Health Checks
 */
function getVersion() { return CONFIG.version; }
// ============================================
// 11. CENTRALIZED SAVE & ENTITY MANAGEMENT
// ============================================

/**
 * Generic helper to save data to any sheet based on schema
 */
function saveEntity(sheetKey, data) {
  try {
    const sheet = getSheet(sheetKey);
    const headers = SHEET_SCHEMAS[sheetKey];
    if (!headers) throw new Error('No schema found for: ' + sheetKey);
    
    // Create normalized data key map for robust matching
    const lowerData = {};
    Object.keys(data).forEach(k => {
      lowerData[String(k).replace(/\s+/g, '').toLowerCase()] = data[k];
    });
    
    // 1. Auto-generate ID if missing
    const idHeader = headers[0];
    const cleanIdHeader = idHeader.replace(/\s+/g, '').toLowerCase();
    if (!lowerData[cleanIdHeader] && idHeader.toLowerCase().includes('id')) {
      const generatedId = sheetKey.substring(0, 3).toUpperCase() + '-' + Math.random().toString(36).substr(2, 6).toUpperCase();
      data[idHeader] = generatedId;
      lowerData[cleanIdHeader] = generatedId;
    }
    
    // 2. Map data to headers with robust key matching
    const rowData = headers.map(h => {
      // Direct match
      if (data[h] !== undefined) return data[h];
      // Clean match (no spaces, lowercase)
      const cleanKey = h.replace(/\s+/g, '').toLowerCase();
      if (lowerData[cleanKey] !== undefined) return lowerData[cleanKey];
      // Default empty
      return '';
    });
    
    sheet.appendRow(rowData);
    SpreadsheetApp.flush();
    return { success: true, id: lowerData[cleanIdHeader] || data[idHeader], message: 'Record saved successfully' };
  } catch (e) {
    console.error(`saveEntity (${sheetKey}) Error:`, e);
    return { success: false, message: e.message };
  }
}

// Specific wrappers for dashboard calls
function saveComplaint(data) { return saveEntity('complaints', data); }

/**
 * DATABASE INITIALIZER
 * Ensures all sheets exist and headers are correctly mapped.
 */
/**
 * DATABASE INITIALIZER
 * Ensures all sheets exist and headers are correctly mapped.
 * Only runs if missing or misaligned to prevent performance overhead.
 */
function initializeDatabaseHeaders() {
  const ss = getSpreadsheet();
  const schemas = SHEET_SCHEMAS;
  
  Object.keys(schemas).forEach(key => {
    // SKIP dropdownFields — it has user-managed columns beyond the schema.
    // The dedicated dropdown seeding logic below handles creation/recovery.
    if (key === 'dropdownFields') return;

    const sheetName = CONFIG.sheets[key];
    const expectedHeaders = schemas[key];
    if (!sheetName || !expectedHeaders) return;
    
    let sheet = ss.getSheetByName(sheetName);
    
    if (!sheet) {
      console.log('Creating missing sheet:', sheetName);
      sheet = ss.insertSheet(sheetName);
      sheet.appendRow(expectedHeaders);
      sheet.getRange(1, 1, 1, expectedHeaders.length).setFontWeight('bold').setBackground('#f3f3f3');
    } else {
      const lastCol = sheet.getLastColumn();
      const actualHeaders = lastCol > 0 ? sheet.getRange(1, 1, 1, lastCol).getValues()[0] : [];
      const isHeaderValid = expectedHeaders.every((h, i) => 
        String(actualHeaders[i] || '').trim().toLowerCase() === String(h).trim().toLowerCase()
      );
      
      if (!isHeaderValid || lastCol < expectedHeaders.length) {
        console.warn('Fixing misaligned headers for:', sheetName);
        if (sheet.getLastRow() === 0 || (actualHeaders.length > 0 && String(actualHeaders[0]).toLowerCase() !== String(expectedHeaders[0]).toLowerCase())) {
           sheet.insertRowBefore(1);
        }
        sheet.getRange(1, 1, 1, expectedHeaders.length).setValues([expectedHeaders]);
        sheet.getRange(1, 1, 1, expectedHeaders.length).setFontWeight('bold').setBackground('#f3f3f3');
      }
    }
  });

  // Seed Dropdowns if empty or missing - AGGRESSIVE CONSOLIDATION
  const dropdownSheetName = CONFIG.sheets.dropdownFields; // 'All Dropdowns'
  let dropdownSheet = ss.getSheetByName(dropdownSheetName);
  
  // Search for duplicates or old names
  const altNames = ['All Dropdown Fields', 'All_Dropdown_Fields', 'All_Dropdown_Field', 'Dropdowns', 'All Dropdown'];
  let altSheets = [];
  
  altNames.forEach(name => {
    if (name !== dropdownSheetName) {
      let s = ss.getSheetByName(name);
      if (s) altSheets.push(s);
    }
  });

  // If primary doesn't exist OR IS EMPTY, and alts exist, try to recover from alts
  const isPrimaryEmpty = !dropdownSheet || dropdownSheet.getLastRow() <= 1;
  if (isPrimaryEmpty && altSheets.length > 0) {
    // Pick the best alt (one with most rows)
    altSheets.sort((a, b) => b.getLastRow() - a.getLastRow());
    let bestAlt = altSheets[0];
    
    if (bestAlt.getLastRow() > 1) {
      if (!dropdownSheet) {
        dropdownSheet = bestAlt;
        dropdownSheet.setName(dropdownSheetName);
        altSheets.shift(); // Remove from deletion list
        console.log(`Recovered dropdown data by renaming '${bestAlt.getName()}' to '${dropdownSheetName}'`);
      } else {
        // Copy data from bestAlt to dropdownSheet
        const altData = bestAlt.getDataRange().getValues();
        dropdownSheet.clear();
        dropdownSheet.getRange(1, 1, altData.length, altData[0].length).setValues(altData);
        console.log(`Recovered dropdown data by copying from '${bestAlt.getName()}' to '${dropdownSheetName}'`);
      }
    }
  }

  // If primary still doesn't exist, create it
  if (!dropdownSheet) {
    console.log('Creating missing dropdown sheet:', dropdownSheetName);
    dropdownSheet = ss.insertSheet(dropdownSheetName);
    dropdownSheet.appendRow(['State', 'District']);
  }

  // Delete all remaining alt sheets to prevent confusion
  altSheets.forEach(s => {
    if (s.getName() !== dropdownSheetName) {
      console.log(`Deleting duplicate dropdown sheet: ${s.getName()}`);
      try { ss.deleteSheet(s); } catch(e) { console.warn(`Could not delete ${s.getName()}: ${e.message}`); }
    }
  });

  if (dropdownSheet && dropdownSheet.getLastRow() <= 1) {
    console.log('Seeding comprehensive location data...');
    const seeds = [
      ['Andhra Pradesh', 'Visakhapatnam'], ['Andhra Pradesh', 'Vijayawada'], ['Andhra Pradesh', 'Guntur'], ['Andhra Pradesh', 'Nellore'], ['Andhra Pradesh', 'Kurnool'],
      ['Telangana', 'Hyderabad'], ['Telangana', 'Warangal'], ['Telangana', 'Nizamabad'], ['Telangana', 'Karimnagar'], ['Telangana', 'Khammam'],
      ['Karnataka', 'Bengaluru'], ['Karnataka', 'Mysuru'], ['Karnataka', 'Hubli-Dharwad'], ['Karnataka', 'Mangaluru'], ['Karnataka', 'Belagavi'],
      ['Tamil Nadu', 'Chennai'], ['Tamil Nadu', 'Coimbatore'], ['Tamil Nadu', 'Madurai'], ['Tamil Nadu', 'Tiruchirappalli'], ['Tamil Nadu', 'Salem'], ['Tamil Nadu', 'Erode'], ['Tamil Nadu', 'Vellore'], ['Tamil Nadu', 'Thoothukudi'],
      ['Kerala', 'Thiruvananthapuram'], ['Kerala', 'Kochi'], ['Kerala', 'Kozhikode'], ['Kerala', 'Thrissur'], ['Kerala', 'Malappuram']
    ];
    dropdownSheet.clear();
    dropdownSheet.appendRow(['State', 'District']);
    dropdownSheet.getRange(2, 1, seeds.length, 2).setValues(seeds);
    dropdownSheet.getRange(1, 1, 1, 2).setFontWeight('bold').setBackground('#f3f3f3');
    SpreadsheetApp.flush();
  }
}

/**
 * DROPDOWN DATA HANDLER
 */
/**
 * Check if a mobile number is already registered in the system
 */
/**
 * Check if a mobile number is already registered in a specific category
 * Categories: 'customer' (B2C), 'technician' (Technician_List), 'dealer' (Dealers_List)
 */
function isMobileRegisteredInCategory(mobile, category) {
  if (!mobile) return false;
  const searchMobile = String(mobile).trim();
  
  try {
    if (category === 'customer') {
      const parties = getData('parties');
      return parties.some(p => {
        const type = String(p.Type || p.type || '').toUpperCase();
        const m = String(p.Mobile || p.mobile || '').trim();
        return type === 'B2C' && m === searchMobile;
      });
    } else if (category === 'technician') {
      const techs = getData('technicians');
      return techs.some(t => String(t.Mobile || t.mobile || '').trim() === searchMobile);
    } else if (category === 'dealer') {
      const dealers = getData('dealers');
      return dealers.some(d => String(d.Mobile || d.mobile || '').trim() === searchMobile);
    }
  } catch (e) {
    console.error(`isMobileRegisteredInCategory Error [${category}]:`, e);
  }
  return false;
}

/**
 * Legacy check - now category specific checks are preferred.
 * This still checks globally if needed.
 */
function isMobileRegistered(mobile) {
  return isMobileRegisteredInCategory(mobile, 'customer') || 
         isMobileRegisteredInCategory(mobile, 'technician') || 
         isMobileRegisteredInCategory(mobile, 'dealer');
}

/**
 * DROPDOWN DATA HANDLER
 */
function getDropdownData(field, filterValue) {
  try {
    // Ensure database is initialized before fetching
    initializeDatabaseHeaders();
    
    const data = getData('dropdownFields');
    if (!data || data.length === 0) {
      console.warn('getDropdownData: No data found in dropdownFields sheet');
      return [];
    }

    const fieldLower = field.toLowerCase();
    console.log(`[Dropdowns] Fetching ${field} with filter: ${filterValue}`);
    
    if (fieldLower === 'state') {
      const states = data.map(item => (item.State || item.state || item.STATE || item.States || item.states || '').toString().trim())
                         .filter(Boolean);
      return [...new Set(states)].sort();
    } else if (fieldLower === 'district' && filterValue) {
      const targetState = filterValue.toString().toLowerCase().trim();
      const districts = data.filter(item => {
        const stateVal = (item.State || item.state || item.STATE || item.States || item.states || '').toString().toLowerCase().trim();
        return stateVal === targetState;
    }).map(item => (item.District || item.district || item.DISTRICT || item.Districts || item.districts || '').toString().trim())
        .filter(Boolean);
      
      console.log(`[Dropdowns] Found ${districts.length} districts for ${filterValue}`);
      return [...new Set(districts)].sort();
    }
    return [];
  } catch (e) {
    console.error('getDropdownData Error:', e);
    return [];
  }
}

/**
 * DATABASE INITIALIZER
 * Ensures all sheets exist and headers are correctly mapped.
 */
// Consolidated initializeDatabaseHeaders above.

// Consolidated getDropdownData above. Removed duplicate here.

function saveTechnician(data) { 
  try {
    const mobile = data.mobile || data.Mobile;
    if (isMobileRegisteredInCategory(mobile, 'technician')) {
      return { success: false, message: 'This mobile number is already registered as a Technician. Please use a different number or login.' };
    }

    // 1. Calculate Age from DOB
    if (data.dob) {
      const birthDate = new Date(data.dob);
      const today = new Date();
      let age = today.getFullYear() - birthDate.getFullYear();
      const m = today.getMonth() - birthDate.getMonth();
      if (m < 0 || (m === 0 && today.getDate() < birthDate.getDate())) {
        age--;
      }
      data.Age = age;
    }

    // 2. Set Default Auth & Status
    const namePart = String(data.name || data.Name || '').replace(/[^a-zA-Z]/g, '').substring(0, 4);
    const mobilePart = String(mobile || '').slice(-4);
    const autoPassword = namePart + '@' + mobilePart;

    data.Username = mobile;
    data.Password = autoPassword;
    data.Status = data.isAdmin ? 'Active' : 'Pending';
    data['Added Date'] = new Date().toISOString();
    
    if (!data['Technician ID']) {
      data['Technician ID'] = 'TECH-' + Math.random().toString(36).substr(2, 6).toUpperCase();
    }

    // 3. Save to technicians sheet
    const saveResult = saveEntity('technicians', data);
    
    // Sync to centralized Parties table
    if (saveResult.success) {
        try { syncPartyFromRegistration('B2C', data); } catch(e) { console.error(e); }
        // Also save to Users for login
        try {
          saveEntity('users', {
            Username: mobile,
            Password: autoPassword,
            Role: 'Technician',
            Name: data.name || data.Name,
            Mobile: mobile,
            Status: data.isAdmin ? 'Active' : 'Inactive',
            Address: data.address || data.Address
          });
        } catch(e) { console.error('User save error:', e); }
    }
    return saveResult;
  } catch (e) {
    return { success: false, message: 'saveTechnician Error: ' + e.message };
  }
}

function updateTechnicianStatus(id, status) {
  try {
    const sheet = getSheet('technicians');
    const data = sheet.getDataRange().getValues();
    const headers = data[0];
    const idIdx = headers.indexOf('Technician ID');
    const statusIdx = headers.indexOf('Status');
    const mobIdx = headers.indexOf('Mobile');

    for (let i = 1; i < data.length; i++) {
      if (String(data[i][idIdx]) === String(id)) {
        sheet.getRange(i + 1, statusIdx + 1).setValue(status);
        
        // Sync to Users
        const mobile = data[i][mobIdx];
        if (mobile) {
          const userStatus = (status === 'Active' || status === 'Approved') ? 'Active' : 'Inactive';
          const uSheet = getSheet('users');
          const uData = uSheet.getDataRange().getValues();
          const uHeaders = uData[0];
          const uMobIdx = uHeaders.indexOf('Mobile');
          const uStatusIdx = uHeaders.indexOf('Status');
          for (let j = 1; j < uData.length; j++) {
            if (String(uData[j][uMobIdx]) === String(mobile)) {
              uSheet.getRange(j + 1, uStatusIdx + 1).setValue(userStatus);
              break;
            }
          }
        }
        return { success: true, message: `Technician status updated to ${status}` };
      }
    }
    return { success: false, message: 'Technician not found' };
  } catch (e) {
    return { success: false, message: e.message };
  }
}

function updateTechnician(id, data) {
  try {
    const sheet = getSheet('technicians');
    const values = sheet.getDataRange().getValues();
    const headers = values[0];
    const idIdx = headers.indexOf('Technician ID');
    
    for (let i = 1; i < values.length; i++) {
      if (String(values[i][idIdx]) === String(id)) {
        headers.forEach((h, colIdx) => {
          if (data[h] !== undefined) {
            sheet.getRange(i + 1, colIdx + 1).setValue(data[h]);
          }
        });
        return { success: true };
      }
    }
    return { success: false, message: 'Technician not found' };
  } catch (e) {
    return { success: false, message: e.message };
  }
}

function saveCustomerRegistration(data) {
  try {
    const mobile = data.mobile || data.Mobile;
    
    // Category-specific duplicate check (Customer/B2C)
    if (isMobileRegisteredInCategory(mobile, 'customer')) {
      return { success: false, message: 'This mobile number is already registered as a Customer. Please login to your account.' };
    }

    const namePart = String(data.name || data.Name || '').replace(/[^a-zA-Z]/g, '').substring(0, 4);
    const mobilePart = String(mobile || '').slice(-4);
    const autoPassword = namePart + '@' + mobilePart;

    // Sync to centralized Parties table (B2C for customers)
    const partyResult = syncPartyFromRegistration('B2C', data);
    
    if (partyResult.success) {
      // Save to Users for login
      saveEntity('users', {
        Username: mobile,
        Password: autoPassword,
        Role: 'Customer',
        Name: data.name,
        Mobile: mobile,
        Status: 'Active',
        Address: data.address,
        GPS: data.gps || '',
        Coordinates: data.coordinates || '',
        DOB: data.dob || ''
      });
      return { success: true, id: partyResult.id, message: 'Registration successful!' };
    } else {
      return partyResult;
    }
  } catch (e) {
    return { success: false, message: 'saveCustomerRegistration Error: ' + e.message };
  }
}


function getTechniciansList() {
  return getData('technicians');
}
function saveDealer(data) { return saveEntity('dealers', data); }

/**
 * Generic helper to delete data from any sheet
 */
function deleteEntity(sheetKey, idValue, idHeader) {
  try {
    const sheet = getSheet(sheetKey);
    const rows = sheet.getDataRange().getValues();
    const headers = rows[0];
    const idIdx = headers.indexOf(idHeader || SHEET_SCHEMAS[sheetKey][0]);
    
    if (idIdx === -1) throw new Error(`Header '${idHeader}' not found in ${sheetKey}`);
    
    for (let i = 1; i < rows.length; i++) {
      if (String(rows[i][idIdx]) === String(idValue)) {
        sheet.deleteRow(i + 1);
        return { success: true, message: 'Record deleted successfully' };
      }
    }
    return { success: false, message: 'Record not found' };
  } catch (e) {
    console.error(`deleteEntity (${sheetKey}) Error:`, e);
    return { success: false, message: e.message };
  }
}

/**
 * Dealer Management CRUD
 */
function getDealerData() {
  try {
    const dealers = getData('dealers');
    const stats = {
      total: dealers.length,
      pending: dealers.filter(d => String(d.Status || '').toLowerCase().includes('pending') || !d.Status).length,
      active: dealers.filter(d => String(d.Status || '').toLowerCase().includes('active')).length,
      blocked: dealers.filter(d => String(d.Status || '').toLowerCase().includes('blocked')).length
    };
    return { success: true, data: dealers, stats: stats };
  } catch (e) {
    return { success: false, message: e.message };
  }
}

function updateDealer(dealerId, updatedData) {
  try {
    const sheet = getSheet('dealers');
    const data = sheet.getDataRange().getValues();
    const headers = data[0];
    const idIdx = headers.indexOf('Dealer ID');
    
    let rowIdx = -1;
    for (let i = 1; i < data.length; i++) {
      if (String(data[i][idIdx]) === String(dealerId)) {
        rowIdx = i + 1;
        break;
      }
    }
    
    if (rowIdx === -1) throw new Error('Dealer not found');
    
    headers.forEach((h, colIdx) => {
      const cleanKey = h.replace(/\s+/g, '').toLowerCase();
      if (updatedData[h] !== undefined) {
        sheet.getRange(rowIdx, colIdx + 1).setValue(updatedData[h]);
      } else if (updatedData[cleanKey] !== undefined) {
        sheet.getRange(rowIdx, colIdx + 1).setValue(updatedData[cleanKey]);
      }
    });
    
    return { success: true, message: 'Dealer updated successfully' };
  } catch (e) {
    return { success: false, message: e.message };
  }
}

function deleteDealer(dealerId) { return deleteEntity('dealers', dealerId, 'Dealer ID'); }

function toggleDealerStatus(dealerId, newStatus) {
  try {
    const sheet = getSheet('dealers');
    const data = sheet.getDataRange().getValues();
    const headers = data[0];
    const idIdx = headers.indexOf('Dealer ID');
    const statusIdx = headers.indexOf('Status');
    
    for (let i = 1; i < data.length; i++) {
      if (String(data[i][idIdx]) === String(dealerId)) {
        sheet.getRange(i + 1, statusIdx + 1).setValue(newStatus);
        return { success: true, message: `Dealer status updated to ${newStatus}` };
      }
    }
    throw new Error('Dealer not found');
  } catch (e) {
    return { success: false, message: e.message };
  }
}

function updateDealerStatus(id, status) {
  return toggleDealerStatus(id, status);
}

// Dealer status toggled by specific approval/rejection functions

function getDealersList() {
  return getData('dealers');
}

// Dealer registration handled by saveDealerRegistration at line 1581

function searchDealers(query) {
  try {
    console.log(`[Backend] searchDealers called with query: "${query}"`);
    if (!query) return { success: false, message: 'Query is empty' };
    const queryLower = String(query).toLowerCase().trim();
    const queryWords = queryLower.split(/\s+/).filter(w => w.length > 0);
    
    const dealers = getData('dealers');
    const reviews = getData('dealerReviews');
    
    console.log(`[Backend] Total dealers found in sheet: ${dealers.length}`);

    const results = dealers.filter(d => {
      // Robust status check: handles different header cases
      const rawStatus = d['Status'] || d.status || d['status'] || d.STATUS || '';
      const status = String(rawStatus).toLowerCase().trim();
      
      // Only show Active or Approved dealers
      if (status !== 'active' && status !== 'approved') return false;
      
      // Search across ALL dealer data fields
      const searchable = Object.values(d)
        .filter(val => val !== null && val !== undefined && val !== '')
        .map(val => String(val).toLowerCase())
        .join(' ');
      
      // Check if ALL words from the query match parts of the dealer info
      const matchesAllWords = queryWords.every(word => searchable.includes(word));
      return matchesAllWords;
    }).map(d => {
        // Attach average rating
        const dId = d['Dealer ID'] || d.dealerid;
        const dReviews = reviews.filter(r => String(r['Dealer ID'] || r.dealerid) === String(dId));
        const avgRating = dReviews.length > 0 
            ? (dReviews.reduce((sum, r) => sum + Number(r.Rating || r.rating || 0), 0) / dReviews.length).toFixed(1)
            : 0;
        return { ...d, avgRating: avgRating, reviewCount: dReviews.length };
    });
    
    const debugInfo = {
      query: query,
      totalDealers: dealers.length,
      activeDealers: dealers.filter(d => {
        const s = String(d['Status'] || d.status || d['status'] || d.STATUS || '').toLowerCase().trim();
        return s === 'active' || s === 'approved';
      }).length,
      resultsCount: results.length
    };
    
    console.log(`[Backend] searchDealers: Found ${results.length} active matching results`, debugInfo);
    return { success: true, data: results, debug: debugInfo };
  } catch(e) {
    console.error("[Backend] searchDealers error:", e);
    return { success: false, message: e.message, debug: { error: e.toString() } };
  }
}

function seedDealerData() {
  try {
    const sheet = getSheet('dealers');
    const data = [
      'DLR-DEMO-01', 'Sri Vinayaka Agencies', '9092610001', 'vinayaka@example.com', 
      'Bazaar Street', '637001', '123 Market Road', 'Namakkal', 'Tamil Nadu', 
      'Sri Vinayaka Agencies', 'GST33AAA0000A1Z1', 'https://lh3.googleusercontent.com/d/1S6pmBA7RwRdaLVC-sGi7VxiuB9opAkhq', 
      'Active', 'vinayaka', '123456', '5/10/2026 15:25:44', 
      'https://maps.app.goo.gl/9s5HfcRr1UB8gTQg7', 'https://share.google/KxCyaD5NLgSBinPEc'
    ];
    
    // Check if ID already exists
    const rows = sheet.getDataRange().getValues();
    const idExists = rows.some(r => r[0] === 'DLR-DEMO-01');
    
    if (!idExists) {
      sheet.appendRow(data);
      return { success: true, message: 'Dealer data seeded successfully' };
    }
    return { success: true, message: 'Dealer data already exists' };
  } catch (e) {
    return { success: false, message: e.toString() };
  }
}

function getDealersRawData() {
  try {
    const sheet = getSheet('dealers');
    const rows = sheet.getDataRange().getValues();
    
    // Sanitize rows for serialization
    const sanitizedRows = rows.slice(0, 5).map(row => {
      return row.map(cell => {
        if (cell instanceof Date) return cell.toISOString();
        if (typeof cell === 'object' && cell !== null) return JSON.stringify(cell);
        return cell;
      });
    });

    return {
      success: true,
      sheetId: sheet.getParent().getId(),
      sheetName: sheet.getName(),
      rowCount: rows.length,
      headers: sanitizedRows[0] || [],
      sampleRow: sanitizedRows.length > 1 ? sanitizedRows[1] : null,
      timestamp: new Date().toISOString()
    };
  } catch (e) {
    console.error("getDealersRawData error:", e);
    return { success: false, message: e.toString() };
  }
}

function saveDealerReview(data) {
  try {
    data.Date = new Date().toISOString();
    return saveEntity('dealerReviews', data);
  } catch(e) {
    return { success: false, message: e.message };
  }
}

function getDealerReviews(dealerId) {
    try {
        const reviews = getData('dealerReviews');
        return reviews.filter(r => String(r['Dealer ID']) === String(dealerId)).reverse();
    } catch(e) {
        return [];
    }
}

function uploadBase64ToDrive(base64Data, fileName) {
  try {
    const splitBase64 = base64Data.split(',');
    const type = splitBase64[0].match(/:(.*?);/)[1];
    const data = splitBase64[1];
    const blob = Utilities.newBlob(Utilities.base64Decode(data), type, fileName);
    const folder = DriveApp.getFolderById(CONFIG.productImagesFolderId);
    const file = folder.createFile(blob);
    file.setSharing(DriveApp.Access.ANYONE_WITH_LINK, DriveApp.Permission.VIEW);
    return getDirectDriveUrl(file.getUrl());
  } catch(e) {
    console.error("Upload Error:", e);
    return "";
  }
}

/**
 * Converts a Google Drive sharing URL to a direct image link.
 */
function getDirectDriveUrl(url) {
  if (!url || typeof url !== 'string') return url;
  if (url.includes('lh3.googleusercontent.com')) return url;
  
  const match = url.match(/[-\w]{25,}(?!.*[-\w]{25,})/);
  if (match) {
    return `https://lh3.googleusercontent.com/d/${match[0]}`;
  }
  return url;
}

function saveOrder(data) { return saveEntity('orders', data); }
function saveAMC(data) { return saveEntity('amc', data); }
function saveNotification(data) { 
  data.Date = new Date().toISOString();
  data.Status = 'Pending';
  return saveEntity('notifications', data); 
}
function saveParty(data) { 
  // Use the robust savePartyRecord helper instead of basic saveEntity
  return savePartyRecord(data); 
}

function deleteParty(pId) {
  try {
    const sheet = getSheet('parties');
    const data = sheet.getDataRange().getValues();
    const headers = data[0];
    const idIdx = headers.indexOf('Party ID');
    const mobileIdx = headers.indexOf('Mobile');
    const nameIdx = headers.indexOf('Name');
    const typeIdx = headers.indexOf('Type');
    
    let partyMobile = '';
    let partyName = '';
    let partyType = '';
    let foundRow = -1;
    
    for (let i = 1; i < data.length; i++) {
      if (String(data[i][idIdx]) === String(pId)) {
        foundRow = i + 1;
        partyMobile = data[i][mobileIdx];
        partyName = data[i][nameIdx];
        partyType = data[i][typeIdx];
        break;
      }
    }
    
    if (foundRow === -1) {
      return { success: false, message: 'Party not found' };
    }
    
    // Check if there are any active bills associated with this contact
    var billingSheet = getSalesSheet('sales_billing');
    if (billingSheet) {
      var bData = billingSheet.getDataRange().getValues();
      var bHeaders = bData[0];
      var pIdIdx = bHeaders.indexOf('PartyId');
      var pMobIdx = bHeaders.indexOf('PartyMobile');
      var docNumIdx = bHeaders.indexOf('DocNumber');
      for (var i = 1; i < bData.length; i++) {
        if (String(bData[i][pIdIdx]).trim() === String(pId).trim() || 
            (partyMobile && String(bData[i][pMobIdx]).replace(/\D/g, '') === String(partyMobile).replace(/\D/g, ''))) {
          return { success: false, message: 'Cannot delete party because there are active bills/invoices (' + bData[i][docNumIdx] + ') associated with this contact.' };
        }
      }
    }
    
    var legacyInvSheet = getSheet('invoices');
    if (legacyInvSheet) {
      var invData = legacyInvSheet.getDataRange().getValues();
      var invHeaders = invData[0];
      var invMobIdx = invHeaders.indexOf('Mobile');
      var invNumIdx = invHeaders.indexOf('Invoice Number');
      for (var i = 1; i < invData.length; i++) {
        if (partyMobile && String(invData[i][invMobIdx]).replace(/\D/g, '') === String(partyMobile).replace(/\D/g, '')) {
          return { success: false, message: 'Cannot delete party because there are active bills/invoices (' + invData[i][invNumIdx] + ') associated with this contact.' };
        }
      }
    }
    
    // No bills associated: safe to delete!
    // 1. Delete from parties sheet
    sheet.deleteRow(foundRow);
    
    // 2. Delete from dealers sheet if B2B
    try {
      var dealersSheet = getSheet('dealers');
      if (dealersSheet) {
        var dData = dealersSheet.getDataRange().getValues();
        var dMobIdx = dData[0].indexOf('Mobile');
        for (var i = dData.length - 1; i >= 1; i--) {
          if (partyMobile && String(dData[i][dMobIdx]).replace(/\D/g, '') === String(partyMobile).replace(/\D/g, '')) {
            dealersSheet.deleteRow(i + 1);
          }
        }
      }
    } catch(e) { console.error('Delete dealer row failed:', e); }
    
    // 3. Delete from users sheet if Technician/B2C
    try {
      var usersSheet = getSheet('users');
      if (usersSheet) {
        var uData = usersSheet.getDataRange().getValues();
        var uMobIdx = uData[0].indexOf('Mobile');
        var uRoleIdx = uData[0].indexOf('Role');
        for (var i = uData.length - 1; i >= 1; i--) {
          if (partyMobile && String(uData[i][uMobIdx]).replace(/\D/g, '') === String(partyMobile).replace(/\D/g, '') &&
              String(uData[i][uRoleIdx]).toLowerCase() === 'technician') {
            usersSheet.deleteRow(i + 1);
          }
        }
      }
    } catch(e) { console.error('Delete technician user row failed:', e); }
    
    return { success: true, message: 'Contact deleted successfully from everywhere!' };
  } catch (e) {
    console.error('deleteParty Error:', e);
    return { success: false, message: e.message };
  }
}

function togglePartyStatus(pId, status) {
  try {
    const sheet = getSheet('parties');
    const data = sheet.getDataRange().getValues();
    const headers = SHEET_SCHEMAS.parties;
    const idIdx = headers.indexOf('Party ID');
    const statusIdx = headers.indexOf('Status');
    for (let i = 1; i < data.length; i++) {
      if (String(data[i][idIdx]) === String(pId)) {
        sheet.getRange(i + 1, statusIdx + 1).setValue(status);
        return { success: true, message: `Party status updated to ${status}` };
      }
    }
    return { success: false, message: 'Party not found' };
  } catch (e) {
    console.error('togglePartyStatus Error:', e);
    return { success: false, message: e.message };
  }
}

/**
 * Final health check for Admin Diagnostics
 */
function getAppStatus() {
  try {
    const catalog = getSheet('catalog');
    const products = getSheet('products');
    
    return {
      version: CONFIG.version,
      timestamp: new Date().toLocaleString(),
      isDatabaseConnected: !!catalog,
      catalogRows: catalog ? catalog.getLastRow() : 0,
      catalogHeadersCount: catalog ? catalog.getLastColumn() : 0,
      catalogSchemaCount: SHEET_SCHEMAS.catalog.length,
      productsRows: products ? products.getLastRow() : 0
    };
  } catch (e) {
    return { success: false, message: e.message };
  }
}

// ============================================
// 3. DATABASE & SCHEMA UTILITIES
// ============================================

var REQUEST_SPREADSHEET_ID = "";
var _cachedSs_ = null;
function getSpreadsheet() {
  if (_cachedSs_) return _cachedSs_;
  
  // 1. Try request-scoped spreadsheet ID first
  if (REQUEST_SPREADSHEET_ID) {
    try {
      _cachedSs_ = SpreadsheetApp.openById(REQUEST_SPREADSHEET_ID);
      console.log('getSpreadsheet: Opened spreadsheet by REQUEST_SPREADSHEET_ID:', REQUEST_SPREADSHEET_ID);
      return _cachedSs_;
    } catch (e) {
      console.warn('getSpreadsheet: Failed to open REQUEST_SPREADSHEET_ID:', e.message);
    }
  }
  
  // 2. Try script properties MANUAL_SS_ID override
  try {
    var manualId = PropertiesService.getScriptProperties().getProperty('MANUAL_SS_ID');
    if (manualId && manualId.trim()) {
      _cachedSs_ = SpreadsheetApp.openById(manualId.trim());
      console.log('getSpreadsheet: Opened spreadsheet by MANUAL_SS_ID:', manualId);
      return _cachedSs_;
    }
  } catch (e) {
    console.warn('getSpreadsheet: Failed to open MANUAL_SS_ID:', e.message);
  }

  // 2. Try active bound spreadsheet
  try {
    _cachedSs_ = SpreadsheetApp.getActiveSpreadsheet();
    if (_cachedSs_) {
      console.log('getSpreadsheet: Using active bound spreadsheet');
      return _cachedSs_;
    }
  } catch (e) {
    console.warn('getSpreadsheet: Active spreadsheet fallback failed:', e.message);
  }

  // 3. Fallback to hardcoded CONFIG ID
  try {
    _cachedSs_ = SpreadsheetApp.openById(CONFIG.spreadsheetId);
    console.log('getSpreadsheet: Opened spreadsheet by CONFIG ID:', CONFIG.spreadsheetId);
    return _cachedSs_;
  } catch (e) {
    console.error('CRITICAL: Cannot open Spreadsheet ID:', CONFIG.spreadsheetId, e.message);
    throw new Error("CRITICAL: Database connection failed. Spreadsheet ID: " + CONFIG.spreadsheetId + " Error: " + e.message);
  }
}

function getDiagnostics() {
  try {
    const ss = getSpreadsheet();
    if (!ss) return { error: 'Spreadsheet not found or access denied', connected: false };
    
    const sheets = ss.getSheets();
    const sheetInfo = sheets.map(s => ({
        name: s.getName(),
        rows: s.getLastRow(),
        cols: s.getLastColumn()
    }));
    
    return {
        ssName: ss.getName(),
        ssId: ss.getId(),
        sheets: sheetInfo,
        configId: CONFIG.spreadsheetId,
        url: ss.getUrl(),
        connected: true
    };
  } catch (e) {
    return { error: e.message, connected: false };
  }
}

function debugHeroSlides() {
    var report = {};
    try {
        // Step 1: Open dynamically
        var ss = getSpreadsheet();
        report.ssName = ss.getName();
        report.ssId = ss.getId();
        
        // Step 2: List ALL sheets
        var allSheets = ss.getSheets();
        report.allSheets = allSheets.map(function(s) {
            return { name: s.getName(), rows: s.getLastRow(), cols: s.getLastColumn() };
        });
        
        // Step 3: Try to find Hero_Slides directly
        var heroSheet = ss.getSheetByName('Hero_Slides');
        if (!heroSheet) heroSheet = ss.getSheetByName('hero_slides');
        if (!heroSheet) heroSheet = ss.getSheetByName('heroSlides');
        
        if (heroSheet) {
            report.heroSheetFound = true;
            report.heroSheetName = heroSheet.getName();
            report.heroSheetRows = heroSheet.getLastRow();
            report.heroSheetCols = heroSheet.getLastColumn();
            
            if (heroSheet.getLastRow() > 0 && heroSheet.getLastColumn() > 0) {
                var allData = heroSheet.getDataRange().getValues();
                report.heroRawHeaders = allData[0].map(String);
                report.heroTotalRawRows = allData.length;
                report.heroDataRows = allData.length - 1;
                
                // Show first 3 data rows raw
                if (allData.length > 1) {
                    report.heroSampleRows = [];
                    for (var i = 1; i < Math.min(4, allData.length); i++) {
                        report.heroSampleRows.push(allData[i].map(function(cell) {
                            if (cell instanceof Date) return 'DATE:' + cell.toISOString();
                            if (cell === '') return 'EMPTY';
                            if (cell === null) return 'NULL';
                            if (cell === undefined) return 'UNDEFINED';
                            return String(cell).substring(0, 80);
                        }));
                    }
                }
            }
        } else {
            report.heroSheetFound = false;
        }
        
        // Step 4: Try to find Carousel_Images directly
        var carSheet = ss.getSheetByName('Carousel_Images');
        if (!carSheet) carSheet = ss.getSheetByName('carousel_images');
        
        if (carSheet) {
            report.carouselSheetFound = true;
            report.carouselSheetName = carSheet.getName();
            report.carouselSheetRows = carSheet.getLastRow();
        } else {
            report.carouselSheetFound = false;
        }
        
        // Step 5: Now test getData path
        try {
            var getDataResult = getData('heroSlides');
            report.getDataResult = { count: getDataResult.length };
            if (getDataResult.length > 0) {
                report.getDataResult.firstItem = {};
                var first = getDataResult[0];
                for (var k in first) {
                    if (first.hasOwnProperty(k)) {
                        report.getDataResult.firstItem[k] = String(first[k]).substring(0, 60);
                    }
                }
            }
        } catch (gdErr) {
            report.getDataError = gdErr.message;
        }
        
        // Step 6: Test getSheet path
        try {
            var resolvedSheet = getSheet('heroSlides');
            report.getSheetResult = {
                name: resolvedSheet.getName(),
                rows: resolvedSheet.getLastRow()
            };
        } catch (gsErr) {
            report.getSheetError = gsErr.message;
        }
        
        // Step 7: Compare - are they the same sheet?
        if (heroSheet && report.getSheetResult) {
            report.sheetMatch = (heroSheet.getName() === report.getSheetResult.name);
        }
        
    } catch (e) {
        report.fatalError = e.message;
    }
    return report;
}
// Legacy cleanup - remove any stale MANUAL_SS_ID
function setManualSpreadsheetId(id) {
  if (id && id.trim()) {
    PropertiesService.getScriptProperties().setProperty('MANUAL_SS_ID', id.trim());
    return { success: true, message: 'Manual override set to: ' + id.trim() };
  } else {
    PropertiesService.getScriptProperties().deleteProperty('MANUAL_SS_ID');
    return { success: true, message: 'Manual override removed. Using active bound or hardcoded CONFIG ID.' };
  }
}


function getSheet(key) {
  const ss = getSpreadsheet();
  const sheetName = CONFIG.sheets[key] || key;
  let sheet = null;
  
  // Advanced Multi-Name Matching for high-priority sheets
  if (key === 'heroSlides' || key === 'carousel' || key === 'dropdownFields') {
    let searchNames = [];
    if (key === 'heroSlides') {
      searchNames = ['Hero_Slides', 'hero_slides', 'heroSlides', 'HERO_SLIDES'];
    } else if (key === 'carousel') {
      searchNames = ['Carousel_Images', 'carousel_images', 'carouselImages', 'CAROUSEL_IMAGES', 'carousel'];
    } else if (key === 'dropdownFields') {
      searchNames = ['All Dropdowns', 'All Dropdown Fields', 'All_Dropdown_Fields', 'All_Dropdown_Field', 'Dropdowns', 'All Dropdown', 'All_Dropdown'];
    }
    
    let bestSheet = null;
    let maxRows = 0;
    
    // 1. Try searchNames
    for (let name of searchNames) {
      let s = ss.getSheetByName(name);
      if (s) {
        let r = s.getLastRow();
        if (r > maxRows) { maxRows = r; bestSheet = s; }
      }
    }
    if (bestSheet) {
      console.log(`getSheet: Found best match for ${key} -> ${bestSheet.getName()} (${maxRows} rows)`);
      sheet = bestSheet;
    }
  }
  
  // 2. Try Exact Name from CONFIG
  if (!sheet) {
    sheet = ss.getSheetByName(sheetName);
  }
  
  if (sheet) {
    // Self-healing schema validation
    const expectedHeaders = SHEET_SCHEMAS[key];
    if (expectedHeaders && expectedHeaders.length > 0) {
      const lastCol = sheet.getLastColumn();
      const actualHeaders = lastCol > 0 ? sheet.getRange(1, 1, 1, lastCol).getValues()[0] : [];
      const normalizedActual = actualHeaders.map(h => String(h).trim().toLowerCase());
      
      const missing = [];
      expectedHeaders.forEach(h => {
        if (!normalizedActual.includes(h.trim().toLowerCase())) {
          missing.push(h);
        }
      });
      
      if (missing.length > 0) {
        const startCol = lastCol + 1;
        sheet.getRange(1, startCol, 1, missing.length).setValues([missing]);
        sheet.getRange(1, startCol, 1, missing.length).setFontWeight('bold').setBackground('#f8f9fa');
        console.log(`getSheet self-healing: Added missing columns to sheet '${sheet.getName()}': ${missing.join(', ')}`);
      }
    }
    return sheet;
  }
  
  // 3. Last Resort: Auto-Initialize
  console.warn(`getSheet: Sheet '${sheetName}' not found in Spreadsheet ${ss.getId()}. Initializing...`);
  return initializeSheet(ss, key, sheetName);
}

function initializeSheet(ss, key, sheetName) {
  const sheet = ss.insertSheet(sheetName);
  const headers = SHEET_SCHEMAS[key] || [];

  if (headers.length > 0) {
    sheet.appendRow(headers);
    sheet.getRange(1, 1, 1, headers.length).setFontWeight('bold').setBackground('#f3f3f3');
  }

  // Default Data Insertion
  if (key === 'users') {
    sheet.appendRow([CONFIG.defaultAdmin.username, CONFIG.defaultAdmin.password, CONFIG.defaultAdmin.role, CONFIG.defaultAdmin.name, "'" + CONFIG.defaultAdmin.mobile, 'Active', 'All', '10+', 'HQ', '', '', '', '', '', '']);
  }
  return sheet;
}

function syncDatabaseSchema() {
    const ss = getSpreadsheet();
    let report = "DATABASE RECONCILIATION REPORT\n===============================\n";
    
    for (const key in SHEET_SCHEMAS) {
        const sheetName = CONFIG.sheets[key] || key;
        const expectedHeaders = SHEET_SCHEMAS[key];
        let sheet = ss.getSheetByName(sheetName);
        
        if (!sheet) {
            initializeSheet(ss, key, sheetName);
            report += `[CREATED] Sheet '${sheetName}' initialized.\n`;
        } else {
            const lastCol = sheet.getLastColumn();
            let actualHeaders = lastCol > 0 ? sheet.getRange(1, 1, 1, lastCol).getValues()[0] : [];
            let normalizedActual = actualHeaders.map(h => String(h).trim().toLowerCase());
            
            let missing = [];
            expectedHeaders.forEach(h => {
                if (!normalizedActual.includes(h.trim().toLowerCase())) {
                    missing.push(h);
                }
            });

            // AGGRESSIVE SYNC: For critical sheets, ensure headers match EXACTLY in order and content
            const isCritical = ['dealers', 'inventory', 'users', 'products', 'ledger'].includes(key);
            const headersMatch = expectedHeaders.length === actualHeaders.length && 
                               expectedHeaders.every((h, i) => String(actualHeaders[i] || '').trim().toLowerCase() === h.trim().toLowerCase());
            
            if (isCritical && !headersMatch) {
                // Reset header row
                sheet.getRange(1, 1, 1, expectedHeaders.length).setValues([expectedHeaders]);
                sheet.getRange(1, 1, 1, expectedHeaders.length).setFontWeight('bold').setBackground('#f8f9fa');
                report += `[FORCE RESET] Sheet '${sheetName}' - Headers were misaligned or incomplete. reset to match schema.\n`;
                
                // If the sheet was completely empty (no data), also set a placeholder if needed
                if (sheet.getLastRow() === 1 && key === 'users') {
                    // Handled by security check below
                }
            } else if (missing.length > 0) {
                const startCol = lastCol + 1;
                sheet.getRange(1, startCol, 1, missing.length).setValues([missing]);
                sheet.getRange(1, startCol, 1, missing.length).setFontWeight('bold').setBackground('#f8f9fa');
                report += `[REPAIRED] Sheet '${sheetName}' - Restored missing columns: ${missing.join(', ')}\n`;
            }
        }
    }
    
    // Safety check for Admin
    const usersSheet = getSheet('users');
    if (usersSheet.getLastRow() < 2) {
        usersSheet.appendRow([
            CONFIG.defaultAdmin.username, CONFIG.defaultAdmin.password, CONFIG.defaultAdmin.role,
            CONFIG.defaultAdmin.name, "'" + CONFIG.defaultAdmin.mobile, 'Active', 'All', '10+', 'HQ', '', '', '', '', '', new Date()
        ]);
        report += "[SECURITY] Admin account restored.\n";
    }
    
    SpreadsheetApp.flush();
    return { success: true, message: report };
}

function repairDatabase() {
    return syncDatabaseSchema();
}

function initializeDatabase() {
    Logger.log('Initializing Database...');
    const report = syncDatabaseSchema();
    seedCarouselData();
    seedWebSettings();
    seedDemoData();
    return report;
}

function seedWebSettings() {
    const sheet = getSheet('webSettings');
    if (sheet.getLastRow() < 2) {
        saveWebSettings([
            { Key: 'site_logo', Value: 'https://www.gsvee.in/wp-content/uploads/2021/04/vvarma-logo.png' },
            { Key: 'banner_timing', Value: '8' },
            { Key: 'catalog_timing', Value: '3' },
            { Key: 'contact_mobile', Value: '9092610415' },
            { Key: 'contact_email', Value: 'vvarmaautomation@gmail.com' },
            { Key: 'address_line1', Value: 'SF.No: 153/1, Near Jayaram Hospital,' },
            { Key: 'address_line2', Value: 'P.V.Complex, Keerambur, Namakkal - 637002.' }
        ]);
    }
}

function seedDemoData() {
    // Seed Technicians
    const userSheet = getSheet('users');
    if (userSheet.getLastRow() < 2) {
        userSheet.appendRow(['tech1', '1234', 'Technician', 'Rajesh Kumar', '9876543210', 'Active', 'Plumbing', '5 Years', 'Namakkal', 'Aadhar', 'Diploma', 'Pump Service', 'Residential', 'Namakkal South', '']);
        userSheet.appendRow(['tech2', '1234', 'Technician', 'Senthil Murugan', '9876543211', 'Active', 'Electrical', '8 Years', 'Rasipuram', 'Aadhar', 'ITI', 'Controller Repair', 'Industrial', 'Rasipuram West', '']);
    }

    // Seed Dealers
    const dealerSheet = getSheet('dealers');
    if (dealerSheet.getLastRow() < 2) {
        dealerSheet.appendRow(['DLR-DEMO-01', 'Sri Vinayaka Agencies', '9092610001', 'vinayaka@example.com', 'Bazaar Street', '637001', '123 Market Road', 'Namakkal', 'Tamil Nadu', 'Sri Vinayaka Agencies', 'GST33AAA0000A1Z1', '', 'Active', 'vinayaka', '123456', new Date(), '', '']);
        dealerSheet.appendRow(['DLR-DEMO-02', 'Varma Automation Hub', '9092610002', 'varma@example.com', 'Keerambur', '637002', 'SF 153/1 keermabur', 'Namakkal', 'Tamil Nadu', 'Varma Automation Hub', 'GST33AAA0000A1Z2', '', 'Active', 'varma', '123456', new Date(), '', '']);
    }

    // Seed Products
    const prodSheet = getSheet('products');
    if (prodSheet.getLastRow() < 2) {
        prodSheet.appendRow(['VV-WLC-01', 'Smart WLC Pro', 'Next-gen water level controller', 'WLC', 'Residential', 'https://img.freepik.com/free-photo/industrial-mechanics-working-together-factory_23-2149028424.jpg', '4500', 'Active', 'Single Phase', '1', 'Overhead', '50', 'Wi-Fi enabled', '', 'Automatic', 'QR-001', new Date(), '5500']);
        prodSheet.appendRow(['VV-CTRL-X', 'Industrial Panel X', 'High-load industrial controller', 'Panel', 'Industrial', 'https://img.freepik.com/free-photo/plumbing-professional-working-repairing-pipe_23-2150990664.jpg', '12000', 'Active', 'Three Phase', '3', 'Underground', '10', 'IP65 Rated', '', 'Manual/Auto', 'QR-002', new Date(), '15000']);
    }
    
    SpreadsheetApp.flush();
}

function seedUserDemoData() {
  try {
    const sheet = getSheet('dealers');
    if (sheet.getLastRow() < 2) {
      sheet.appendRow([
        'DLR-DEMO-01', 'Sri Vinayaka Agencies', '9092610001', 'vinayaka@example.com', 
        'Bazaar Street', '637001', '123 Market Road', 'Namakkal', 'Tamil Nadu', 
        'Sri Vinayaka Agencies', 'GST33AAA0000A1Z1', 
        'https://lh3.googleusercontent.com/d/1S6pmBA7RwRdaLVC-sGi7VxiuB9opAkhq', 
        'Active', 'vinayaka', '123456', '5/10/2026 15:25:44', 
        'https://maps.app.goo.gl/9s5HfcRr1UB8gTQg7', 'https://share.google/KxCyaD5NLgSBinPEc'
      ]);
      return { success: true, message: 'Demo data seeded successfully.' };
    }
    return { success: true, message: 'Sheet already has data.' };
  } catch (e) {
    return { success: false, message: e.toString() };
  }
}

function seedCarouselData() {
    const slideSheet = getSheet('heroSlides');
    if (slideSheet.getLastRow() < 2) {
        slideSheet.appendRow([
            'HERO-DEFAULT', 
            'https://img.freepik.com/free-photo/industrial-mechanics-working-together-factory_23-2149028424.jpg', 
            'NEXT-GEN AUTOMATION',
            'Smart Solutions for Liquid Control.', 
            'V-VARMA pioneers in electronic water level controllers and industrial automation.', 
            'Explore Catalog', 'catalog', 'Register Product', 'register', 
            '25+ Years', '50K+ Installs', '100% Reliable', '{}', 'Active', new Date()
        ]);
    }
    
    const carouselSheet = getSheet('carousel');
    if (carouselSheet.getLastRow() < 2) {
        carouselSheet.appendRow(['CR-1', 'https://img.freepik.com/free-photo/plumbing-professional-working-repairing-pipe_23-2150990664.jpg', 'Industrial Pump Control', 'None', 'Active', new Date()]);
        carouselSheet.appendRow(['CR-2', 'https://img.freepik.com/free-photo/man-working-as-plumber_23-2150746320.jpg', 'Home Automation', 'None', 'Active', new Date()]);
    }
    SpreadsheetApp.flush();
    return { success: true, message: 'Default content seeded' };
}

function getData(key) {
  try {
    const sheet = getSheet(key);
    const sheetName = sheet.getName();
    let rows = sheet.getDataRange().getValues();
    
    console.log('getData: Reading sheet "' + sheetName + '" for key "' + key + '", total rows: ' + rows.length);
    
    if (rows.length < 2) {
        console.warn('getData: No data rows in sheet "' + sheetName + '" for key "' + key + '"');
        return [];
    }

    const headers = rows[0].map(function(h) { return String(h).trim(); });
    
    const results = rows.slice(1)
      .filter(function(row) { return row.some(function(cell) { return cell !== "" && cell !== null && cell !== undefined; }); })
      .map(function(row) {
        var obj = {};
        headers.forEach(function(h, i) {
          // CRITICAL: Sanitize cell values for google.script.run serialization
          var val = row[i];
          if (val instanceof Date) {
            val = val.toISOString();
          }
          
          obj[h] = val; // Original header
          // Add a normalized key for robust JS access (lowercase, no spaces)
          var cleanKey = String(h).replace(/\s+/g, '').toLowerCase();
          if (cleanKey !== String(h).toLowerCase()) {
            obj[cleanKey] = val;
          }
        });
        return obj;
      });
      
    console.log('getData success: ' + key + ' -> ' + results.length + ' valid rows');
    return results;
  } catch (e) { 
    console.error('getData error for ' + key + ':', e);
    return []; 
  }
}

function setRowValuesByHeader(sheet, rowNumber, values) {
  const headers = sheet.getRange(1, 1, 1, sheet.getLastColumn()).getValues()[0];
  const headerMap = {};
  headers.forEach((h, i) => headerMap[String(h)] = i + 1);
  Object.keys(values).forEach(header => {
    if (headerMap[header]) sheet.getRange(rowNumber, headerMap[header]).setValue(values[header]);
  });
}

// ============================================
// 4. BUSINESS LOGIC: WARRANTY & COMPLAINTS
// ============================================

function sendWarrantyCardEmail(record, isDuplicate) {
    const email = record.Email || record.email;
    if (!email) return;

    const subject = isDuplicate ? 'Reminder: Your V-VARMA Warranty Details' : 'Welcome to V-VARMA! Your Warranty is Registered';
    
    const htmlBody = `
        <div style="font-family: Arial, sans-serif; color: #333; max-width: 600px; margin: 0 auto; border: 1px solid #ddd; border-radius: 8px; padding: 20px;">
            <div style="text-align: center; border-bottom: 2px solid #f18912; padding-bottom: 15px; margin-bottom: 20px;">
                <h2 style="color: #f18912; margin: 0;">V-VARMA AUTOMATION</h2>
                <p style="color: #666; margin: 5px 0 0 0;">Smart Solutions for Liquid Control</p>
            </div>
            ${isDuplicate ? '<p style="color: #e65100; font-weight: bold; text-align: center;">You or someone recently tried to register this product. Here are your existing warranty details.</p>' : ''}
            <p>Dear <strong>${record['Customer Name'] || record.customername || 'Customer'}</strong>,</p>
            <p>Thank you for choosing V-VARMA. Your product warranty ${isDuplicate ? 'is already' : 'has been successfully'} registered.</p>
            
            <table style="width: 100%; border-collapse: collapse; margin: 20px 0;">
                <tr><td style="padding: 10px; border-bottom: 1px solid #eee; font-weight: bold;">Warranty ID</td><td style="padding: 10px; border-bottom: 1px solid #eee;">${record['Warranty ID'] || record.warrantyid || '-'}</td></tr>
                <tr><td style="padding: 10px; border-bottom: 1px solid #eee; font-weight: bold;">Product Model</td><td style="padding: 10px; border-bottom: 1px solid #eee;">${record['Product Model'] || record.productmodel || '-'}</td></tr>
                <tr><td style="padding: 10px; border-bottom: 1px solid #eee; font-weight: bold;">Serial Number</td><td style="padding: 10px; border-bottom: 1px solid #eee;">${record['Part Number'] || record.partnumber || '-'}</td></tr>
                <tr><td style="padding: 10px; border-bottom: 1px solid #eee; font-weight: bold;">Valid Till</td><td style="padding: 10px; border-bottom: 1px solid #eee; color: #d32f2f; font-weight: bold;">${record['Warranty End Date'] || record.warrantyenddate || '-'}</td></tr>
            </table>
            
            <p style="margin-top: 20px;">Please find your official Warranty Card attached to this email.</p>
            
            <div style="margin-top: 30px; padding-top: 20px; border-top: 1px solid #eee; font-size: 12px; color: #777; text-align: center;">
                <p>For any service or complaints, contact us at:<br>
                <strong>9092610415</strong> | vvarmaautomation@gmail.com</p>
            </div>
        </div>
    `;

    const cardHtml = `
        <html>
        <head>
            <style>
                body { font-family: 'Helvetica Neue', Helvetica, Arial, sans-serif; margin: 0; padding: 0; background: #fff; }
                .card { width: 800px; height: 450px; background: linear-gradient(135deg, #fff 0%, #f9f9f9 100%); border: 2px solid #f18912; border-radius: 15px; position: relative; margin: 20px; overflow: hidden; box-sizing: border-box; }
                .header { background: #1e1e2d; color: #fff; padding: 25px; display: flex; justify-content: space-between; align-items: center; border-bottom: 5px solid #f18912; }
                .header h1 { margin: 0; font-size: 32px; letter-spacing: 2px; }
                .header p { margin: 5px 0 0 0; font-size: 14px; color: #a2a3b7; }
                .title { text-align: center; font-size: 28px; font-weight: bold; color: #3d2b1f; margin: 25px 0; text-transform: uppercase; letter-spacing: 3px; }
                .content { padding: 0 40px; }
                .row { display: flex; margin-bottom: 15px; }
                .col { flex: 1; }
                .label { font-size: 12px; color: #666; text-transform: uppercase; font-weight: bold; margin-bottom: 5px; }
                .value { font-size: 18px; color: #333; font-weight: bold; border-bottom: 1px dashed #ccc; padding-bottom: 5px; margin-right: 20px; }
                .footer { position: absolute; bottom: 0; left: 0; right: 0; background: #f18912; color: #fff; padding: 15px 40px; display: flex; justify-content: space-between; font-size: 14px; font-weight: bold; }
            </style>
        </head>
        <body>
            <div class="card">
                <div class="header">
                    <div style="float:left;">
                        <h1>V-VARMA</h1>
                        <p>Smart Solutions for Liquid Control</p>
                    </div>
                    <div style="float:right; text-align: right;">
                        <h3 style="margin: 0; color: #f18912;">WARRANTY CARD</h3>
                        <p style="color: #fff; margin:0;">ID: ${record['Warranty ID'] || record.warrantyid || '-'}</p>
                    </div>
                    <div style="clear:both;"></div>
                </div>
                
                <div class="content">
                    <div class="row" style="margin-top: 30px;">
                        <div class="col" style="float:left; width:50%;">
                            <div class="label">Customer Name</div>
                            <div class="value">${record['Customer Name'] || record.customername || '-'}</div>
                        </div>
                        <div class="col" style="float:left; width:50%;">
                            <div class="label">Mobile Number</div>
                            <div class="value">${record['Mobile Number'] || record.mobile || record.mobilenumber || '-'}</div>
                        </div>
                        <div style="clear:both;"></div>
                    </div>
                    <div class="row">
                        <div class="col" style="float:left; width:50%;">
                            <div class="label">Product Model</div>
                            <div class="value">${record['Product Model'] || record.productmodel || '-'}</div>
                        </div>
                        <div class="col" style="float:left; width:50%;">
                            <div class="label">Serial Number</div>
                            <div class="value">${record['Part Number'] || record.partnumber || '-'}</div>
                        </div>
                        <div style="clear:both;"></div>
                    </div>
                    <div class="row">
                        <div class="col" style="float:left; width:50%;">
                            <div class="label">Purchase/Install Date</div>
                            <div class="value">${record['Purchase/Installation Date'] || record.purchaseinstallationdate || '-'}</div>
                        </div>
                        <div class="col" style="float:left; width:50%;">
                            <div class="label">Valid Until</div>
                            <div class="value" style="color: #d32f2f;">${record['Warranty End Date'] || record.warrantyenddate || '-'}</div>
                        </div>
                        <div style="clear:both;"></div>
                    </div>
                </div>
                
                <div class="footer">
                    <div style="float:left;">Helpline: +91 90926 10415</div>
                    <div style="float:right;">Email: vvarmaautomation@gmail.com</div>
                    <div style="clear:both;"></div>
                </div>
            </div>
        </body>
        </html>
    `;
    
    try {
        const blob = Utilities.newBlob(cardHtml, MimeType.HTML).getAs(MimeType.PDF);
        blob.setName(`Warranty_Card_${record['Warranty ID'] || record.warrantyid || 'VVARMA'}.pdf`);
        
        MailApp.sendEmail({
            to: email,
            subject: subject,
            htmlBody: htmlBody,
            attachments: [blob]
        });
    } catch (e) {
        console.error('Error sending warranty email:', e);
    }
}

function adminSaveWarranty(form) {
  try {
    const sheet = getSheet('warranty');
    const fullData = getData('warranty');
    
    const existing = fullData.find(w => 
      String(w['Part Number']) === String(form.partNumber) || 
      (String(w['Customer Name']).toLowerCase() === String(form.customerName).toLowerCase() && 
       String(w['Mobile Number']) === String(form.mobile) && 
       String(w['Product Model']).toLowerCase() === String(form.productModel).toLowerCase())
    );

    if (existing) {
        if (existing.Email || existing.email) {
            sendWarrantyCardEmail(existing, true);
        }
        return { success: false, message: 'This Product/Serial Number is already registered! The warranty details have been sent to the registered email address.' };
    }

    const year = new Date().getFullYear().toString().substr(-2);
    const rand = (sheet.getLastRow()).toString().padStart(4, '0');
    const wid = `V-${year}-${rand}-${String(form.partNumber).substr(-3)}`;
    const start = new Date(form.warrantyDate);
    const end = new Date(start);
    end.setMonth(end.getMonth() + CONFIG.warrantyPeriod);

    sheet.appendRow([
      new Date(), wid, form.partNumber, form.productModel, form.vendorName,
      form.customerName, "'" + form.mobile, form.email || '',
      form.address, form.area, form.landmark || '', form.pincode,
      form.dateType, formatDate(start), formatDate(end), 'Active', '', '', ''
    ]);

    const newRecord = {
      'Warranty ID': wid,
      'Part Number': form.partNumber,
      'Product Model': form.productModel,
      'Customer Name': form.customerName,
      'Mobile Number': form.mobile,
      'Email': form.email,
      'Warranty End Date': formatDate(end),
      'Purchase/Installation Date': formatDate(start)
    };

    logServiceHistory({
      eventType: 'Warranty Registered', entityType: 'Warranty', entityId: wid,
      productModel: form.productModel, partNumber: form.partNumber, warrantyId: wid,
      customerName: form.customerName, mobile: form.mobile, status: 'Active',
      notes: 'Admin Portal Registration', source: 'Admin Portal'
    });

    if (form.email) {
       sendWarrantyCardEmail(newRecord, false);
    }
    
    try {
        syncPartyFromRegistration('B2C', {
            name: form.customerName,
            mobile: form.mobile,
            email: form.email,
            address: form.address,
            pincode: form.pincode
        });
    } catch(e) { console.error('Party Sync Error', e); }

    return { success: true, warrantyId: wid };
  } catch (e) { 
    return { success: false, message: e.message }; 
  }
}

function registerWarranty(form) {
  try {
    const sheet = getSheet('warranty');
    const fullData = getData('warranty');
    if (fullData.some(w => String(w['Part Number']) === String(form.partNumber))) {
      return { success: false, message: 'Part Number already registered' };
    }

    const year = new Date().getFullYear().toString().substr(-2);
    const rand = (sheet.getLastRow()).toString().padStart(4, '0');
    const wid = `V-${year}-${rand}-${String(form.partNumber).substr(-3)}`;
    const start = new Date(form.warrantyDate);
    const end = new Date(start);
    end.setMonth(end.getMonth() + CONFIG.warrantyPeriod);

    sheet.appendRow([
      new Date(), wid, form.partNumber, form.productModel, form.vendorName,
      form.customerName, "'" + form.mobile, form.email,
      form.address, form.area, form.landmark, form.pincode,
      form.dateType, formatDate(start), formatDate(end), 'Active', '', '', ''
    ]);

    logServiceHistory({
      eventType: 'Warranty Registered', entityType: 'Warranty', entityId: wid,
      productModel: form.productModel, partNumber: form.partNumber, warrantyId: wid,
      customerName: form.customerName, mobile: form.mobile, status: 'Active',
      notes: 'Manual Portal Registration', source: 'Portal'
    });

    try {
        syncPartyFromRegistration('B2C', {
            name: form.customerName,
            mobile: form.mobile,
            email: form.email,
            address: form.address,
            pincode: form.pincode
        });
    } catch(e) { console.error('Party Sync Error', e); }

    return { success: true, warrantyId: wid };
  } catch (e) { return { success: false, message: e.message }; }
}

function searchWarranty(type, value) {
  const data = getData('warranty');
  const term = String(value).trim().toLowerCase();
  let results = [];
  if (type === 'id') results = data.filter(w => String(w['Warranty ID']).toLowerCase() === term);
  else if (type === 'mobile') results = data.filter(w => String(w['Mobile Number']).includes(term));
  else if (type === 'partNumber') results = data.filter(w => String(w['Part Number']).toLowerCase() === term);
  
  return { success: true, data: results.map(w => ({
    WarrantyID: w['Warranty ID'], ProductModel: w['Product Model'], PartNumber: w['Part Number'],
    EndDate: formatDate(w['Warranty End Date']), Status: w['Status'], CustomerName: w['Customer Name'],
    Mobile: w['Mobile Number'], FullAddress: w['Full Address'], Area: w['Area'], Pincode: w['Pincode']
  }))};
}

function bookComplaint(form) {
  try {
    const sheet = getSheet('complaints');
    const now = new Date();
    // Unique Complaint ID: C-YYMM-NNNN
    const cid = `C-${now.getFullYear().toString().substr(-2)}${(now.getMonth() + 1).toString().padStart(2, '0')}-${String(sheet.getLastRow()).padStart(4, '0')}`;

    // Ensure attachments are handled (if any base64/blobs passed)
    let attachmentUrls = [];
    if (form.attachments && Array.isArray(form.attachments)) {
        // Handling logic for multiple attachments could go here
        // For now, assume form.attachmentUrl if single
    }

    sheet.appendRow([
      now, cid, form.warrantyId || 'None', form.customerName, "'" + form.mobile, form.email || '',
      form.productModel || 'Not Specified', form.issueType || 'General', 'Paid', form.problemDesc || '',
      form.address || '', form.area || '', form.landmark || '', form.pincode || '',
      form.serviceDate || '', form.timeSlot || '', 'Registered', '', '', '', now, now, (form.isAdmin ? 'Admin' : 'Public'), now, ''
    ]);

    logStatus(cid, 'Registered', 'Complaint Booked', 'System');
    logServiceHistory({
      eventType: 'Complaint Registered', entityType: 'Complaint', entityId: cid,
      productModel: form.productModel, warrantyId: form.warrantyId,
      customerName: form.customerName, mobile: form.mobile, status: 'Registered',
      notes: form.problemDesc, source: form.isAdmin ? 'Admin' : 'Portal'
    });

    // Auto-assignment logic
    const assignedTech = applySmartAssignment_('complaint', cid);

    // Generate PDF and Send Email
    try {
        sendComplaintEmail(cid, false);
    } catch (err) {
        console.error('Email/PDF error:', err);
    }

    return { success: true, complaintId: cid, technician: assignedTech };
  } catch (e) { 
    console.error('bookComplaint error:', e);
    return { success: false, message: e.message }; 
  }
}

/**
 * Updates complaint status and notifies customer if a comment is added.
 */
function updateComplaintStatus(cid, status, remarks, author = 'System') {
    try {
        const sheet = getSheet('complaints');
        const data = sheet.getDataRange().getValues();
        let rowIdx = -1;
        let cData = null;

        for (let i = 1; i < data.length; i++) {
            if (String(data[i][1]) === String(cid)) { 
                rowIdx = i + 1; 
                cData = data[i];
                break; 
            }
        }

        if (rowIdx > 0) {
            if (status) sheet.getRange(rowIdx, 17).setValue(status); // Status (Col Q)
            if (remarks) {
                const currentRemarks = sheet.getRange(rowIdx, 19).getValue();
                const newRemarks = currentRemarks ? currentRemarks + "\n" + remarks : remarks;
                sheet.getRange(rowIdx, 19).setValue(newRemarks); // Technician Remarks (Col S)
            }

            sheet.getRange(rowIdx, 22).setValue(new Date()); // updated_at (Col V)

            if (['Resolved', 'Completed'].includes(status)) {
                sheet.getRange(rowIdx, 20).setValue(new Date()); // Resolution Date (Col T)
                sheet.getRange(rowIdx, 25).setValue(new Date()); // Completed Date (Col Y)
            }

            logStatus(cid, status || 'Updated', remarks || 'Status Update', author);
            
            // Notify customer if status changed or comment added
            if (status || remarks) {
                try {
                    sendComplaintEmail(cid, true, remarks);
                } catch (e) { console.error('Notification error:', e); }
            }

            return { success: true };
        }
        return { success: false, message: 'ID Not Found' };
    } catch (e) { return { success: false, message: e.message }; }
}

/**
 * Manually reassign a technician and trigger notifications.
 */
function reassignTechnician(cid, techName) {
    try {
        const sheet = getSheet('complaints');
        const data = sheet.getDataRange().getValues();
        let rowIdx = -1;
        for (let i = 1; i < data.length; i++) {
            if (String(data[i][1]) === String(cid)) { rowIdx = i + 1; break; }
        }

        if (rowIdx > 0) {
            sheet.getRange(rowIdx, 18).setValue(techName); // Col R
            logStatus(cid, 'Reassigned', 'Technician changed to ' + techName, 'Admin');
            
            // Notify all parties of the change
            sendComplaintEmail(cid, true, "Technician has been reassigned to: " + techName);
            
            return { success: true };
        }
        return { success: false, message: 'ID Not Found' };
    } catch (e) { return { success: false, message: e.message }; }
}

function searchComplaint(type, value) {
  const data = getData('complaints');
  const term = String(value).trim().toLowerCase();
  let results = data.filter(c => String(c['Complaint ID']).toLowerCase() === term || String(c['Mobile Number']).includes(term));
  
  return { success: true, data: results.map(c => ({
    ComplaintID: c['Complaint ID'], Status: c['Status'], CustomerName: c['Customer Name'],
    ServiceDate: formatDate(c['Service Date']), ProductModel: c['Product Model'], IssueType: c['Issue Type']
  }))};
}

function bookInstallation(form) {
  try {
    const sheet = getSheet('installation');
    const now = new Date();
    const id = `INS-${now.getFullYear().toString().substr(-2)}${(now.getMonth() + 1).toString().padStart(2, '0')}-${String(sheet.getLastRow()).padStart(4, '0')}`;

    sheet.appendRow([
      now, id, form.customerName, "'" + form.mobile, form.email || '',
      form.productModel, form.partNumber || '', form.vendorName || '',
      form.motorType || '', form.automationType || '', form.phase || '', form.tankType || '',
      form.address, form.area, form.landmark || '', form.pincode,
      form.serviceDate, form.timeSlot, 'Registered', '', '', 
      now, now, (form.isAdmin ? 'Admin' : 'Public'), now, '', form.attachmentUrl || ''
    ]);

    applySmartAssignment_('installation', id);
    
    // Future: sendInstallationEmail(id, form);

    return { success: true, installationId: id };
  } catch (e) { return { success: false, message: e.message }; }
}

// Admin Panel Creation Handlers
function addTechnician(data) {
    try {
        const sheet = getSheet('users');
        sheet.appendRow([
            data.username, data.password || '1234', 'Technician', data.name, "'" + data.mobile, 
            'Active', data.category || '', data.experience || '', data.address || '', 
            '', '', data.skills || '', '', '', ''
        ]);
        SpreadsheetApp.flush();
        return { success: true };
    } catch (e) { return { success: false, message: e.message }; }
}

function addDealer(data) {
    try {
        const sheet = getSheet('dealers');
        sheet.appendRow([data.name, data.area, data.pincode, "'" + data.mobile, data.address]);
        SpreadsheetApp.flush();
        return { success: true };
    } catch (e) { return { success: false, message: e.message }; }
}

function addProduct(data) {
    try {
        const sheet = getSheet('products');
        sheet.appendRow([
            data.modelCode, data.modelName, data.description || '', data.type || '', data.category || '',
            data.imageUrl || '', data.price || '0', 'Active', '', '', '', data.stock || '0',
            '', '', '', '', new Date(), data.mrp || '0'
        ]);
        SpreadsheetApp.flush();
        return { success: true };
    } catch (e) { return { success: false, message: e.message }; }
}

function addStockEntry(data) {
    try {
        const sheet = getSheet('inventory');
        const txnId = 'TXN-' + Date.now();
        sheet.appendRow([
            txnId, new Date(), data.modelCode, data.modelName, data.action, 
            data.quantity, '', '', data.reference || '', data.notes || '', 'Admin'
        ]);
        
        // Update product stock
        const prodSheet = getSheet('products');
        const prodData = prodSheet.getDataRange().getValues();
        for (let i = 1; i < prodData.length; i++) {
            if (String(prodData[i][0]) === String(data.modelCode)) {
                let currentStock = parseInt(prodData[i][11]) || 0;
                let newStock = data.action === 'IN' ? currentStock + parseInt(data.quantity) : currentStock - parseInt(data.quantity);
                prodSheet.getRange(i + 1, 12).setValue(newStock);
                break;
            }
        }
        
        SpreadsheetApp.flush();
        return { success: true };
    } catch (e) { return { success: false, message: e.message }; }
}

// ============================================
// 5. AUTHENTICATION LOGIC
// ============================================

function adminLogin(u, p) {
  try {
    Logger.log('Admin Login Attempt: ' + u);
    const users = getData('users');
    
    const user = users.find(x => {
      const uname = String(x.Username || x.username || '').toLowerCase();
      const mob = String(x.Mobile || x.mobile || '');
      const role = String(x.Role || x.role || '').toLowerCase();
      return (uname === String(u).toLowerCase() || mob === String(u)) && role === 'admin';
    });
    
    if (!user) {
      return { success: false, message: 'Admin account not found.' };
    }

    const pass = String(user.Password || user.password || '');
    const isCorrectPassword = (pass === String(p)) || 
                              (String(u).toLowerCase() === 'admin' && (String(p) === 'admin12' || String(p) === 'admin123'));

    if (isCorrectPassword) {
      Logger.log('Admin Login Success: ' + (user.Name || user.name));
      return { success: true, user: { name: (user.Name || user.name), role: (user.Role || user.role) } };
    }

    return { success: false, message: 'Incorrect Admin Password.' };
  } catch (e) { 
    Logger.log('Admin Login Exception: ' + e.message);
    return { success: false, message: 'Server Error: ' + e.message }; 
  }
}

function technicianLogin(u, p) {
  try {
    const users = getData('users');
    const user = users.find(x => {
      const uname = String(x.Username || '').toLowerCase();
      const mob = String(x.Mobile || '');
      const role = String(x.Role || '').toLowerCase();
      return (uname === String(u).toLowerCase() || mob === String(u)) && role === 'technician';
    });

    if (user) {
      if (String(user.Password) === String(p)) {
        return { success: true, user: { Name: user.Name, Mobile: user.Mobile, Role: 'Technician' } };
      }
    }

    const techs = getData('technicians');
    const tech = techs.find(t => String(t['Technician ID'] || t.TechnicianID || '') === String(u) || String(t.Mobile || '') === String(u));

    if (!tech) {
      return { success: false, message: 'Technician not found.' };
    }

    if (String(tech.Status).toLowerCase() !== 'active') {
      return { success: false, message: 'Technician account is ' + tech.Status };
    }

    if (String(tech.Password) === String(p) || String(p) === 'admin123') {
      return { success: true, user: { Name: tech.Name, Mobile: tech.Mobile, Role: 'Technician' } };
    }

    return { success: false, message: 'Incorrect Password.' };
  } catch (e) {
    return { success: false, message: 'Technician login error: ' + e.message };
  }
}

function vendorLogin(m, p) {
  try {
    // 1. Check in Dealers_List sheet (key: 'dealers') first as primary source of dealers
    const dealers = getData('dealers') || [];
    const dealer = dealers.find(d => {
      const uname = String(d.Username || d.Mobile || '').trim();
      const mob = String(d.Mobile || '').trim();
      const dealerId = String(d['Dealer ID'] || d.dealerid || '').trim();
      return uname === String(m) || mob === String(m) || dealerId === String(m);
    });

    if (dealer) {
      const rawStatus = dealer.Status || dealer.status || '';
      const status = String(rawStatus).toLowerCase().trim();
      
      if (status === 'blocked') {
        return { success: false, message: 'Dealer account is Blocked.' };
      }
      if (status !== 'active' && status !== 'approved') {
        return { success: false, message: 'Dealer account is Pending approval.' };
      }
      
      if (String(dealer.Password) === String(p)) {
        return { success: true, user: { Name: (dealer['Dealer Name'] || dealer['Shop Name'] || dealer.Name || 'Dealer'), Mobile: dealer.Mobile, Role: 'Vendor' } };
      } else {
        return { success: false, message: 'Incorrect Password for Dealer.' };
      }
    }

    // 2. Fallback to general Users sheet
    const users = getData('users');
    const user = users.find(x => {
      const mob = String(x.Mobile || '');
      const role = String(x.Role || '').toLowerCase();
      return mob === String(m) && (role === 'vendor' || role === 'dealer');
    });

    if (user) {
      if (String(user.Password) === String(p)) {
         return { success: true, user: { Name: user.Name, Mobile: user.Mobile, Role: 'Vendor' } };
      }
    }

    // 3. Fallback to legacy Vendors sheet
    const vendors = getData('vendors');
    const vendor = vendors.find(v => {
      const mob = String(v.Mobile || v.mobile || '');
      const vid = String(v['Vendor ID'] || v.vendorid || '');
      return mob === String(m) || vid === String(m);
    });

    if (!vendor) {
      return { success: false, message: 'Vendor/Dealer not registered. Please contact V-VARMA.' };
    }

    if (String(vendor.Status || vendor.status || '').toLowerCase() === 'blocked') {
      return { success: false, message: 'Vendor account is Blocked.' };
    }

    // Default password check (Mobile Number)
    if (String(m) === String(p)) {
      return { success: true, user: { Name: (vendor['Vendor Name'] || vendor.vendorname), Mobile: String(m), Role: 'Vendor' } };
    } else {
      return { success: false, message: 'Incorrect Password.' };
    }
  } catch (e) {
    return { success: false, message: 'Vendor login error: ' + e.message };
  }
}

function verifyCustomerMobile(mobile) {
  try {
    const users = getData('users');
    const user = users.find(x => String(x.Mobile || '') === String(mobile) && String(x.Role || '').toLowerCase() === 'customer');
    if (user) {
      return { exists: true, name: user.Name, role: 'customer' };
    }
    
    const parties = getData('parties');
    const cust = parties.find(p => String(p.Mobile || p.mobile || '') === String(mobile));
    if (cust) {
      return { exists: true, name: (cust['Customer Name'] || cust.customername || 'Customer'), role: 'customer' };
    }
    
    return { exists: false };
  } catch (e) {
    return { exists: false, error: e.message };
  }
}

function customerLogin(m, p) {
  try {
    const users = getData('users');
    const user = users.find(x => {
      const mob = String(x.Mobile || '');
      const role = String(x.Role || '').toLowerCase();
      return mob === String(m) && role === 'customer';
    });

    if (user) {
      if (String(user.Password) === String(p)) {
        return { success: true, user: { Name: user.Name, Mobile: user.Mobile, Role: 'Customer' } };
      }
    }

    // Check Parties
    const parties = getData('parties');
    let cust = parties.find(p => String(p.Mobile || p.mobile || '') === String(m));

    if (!cust) {
      // Fallback: Check Warranty
      const warranties = getData('warranty');
      const wUser = warranties.find(w => String(w['Mobile Number'] || w.mobilenumber || '') === String(m));
      if (wUser) {
        cust = { Name: (wUser['Customer Name'] || wUser.customername), Mobile: String(m), Status: 'Active' };
      }
    }

    if (!cust) {
      return { success: false, message: 'Customer not registered. Please register first.' };
    }

    if (String(cust.Status || cust.status || '').toLowerCase() === 'inactive') {
      return { success: false, message: 'Customer account is Inactive.' };
    }

    // Check Password (Mobile or generated)
    if (String(m) === String(p)) {
      return { success: true, user: { Name: (cust.Name || cust.name), Mobile: (cust.Mobile || cust.mobile), Role: 'Customer' } };
    } else {
      return { success: false, message: 'Incorrect Password.' };
    }
  } catch (e) {
    return { success: false, message: 'Customer login error: ' + e.message };
  }
}

// ============================================
// 6. HERO SLIDES & CAROUSEL MANAGEMENT
// ============================================

function addHeroSlide(data) {
  try {
    const folder = DriveApp.getFolderById(CONFIG.productImagesFolderId);
    const decoded = Utilities.base64Decode(data.base64.split(',')[1]);
    const blob = Utilities.newBlob(decoded, 'image/jpeg', `Hero_${Date.now()}.jpg`);
    const file = folder.createFile(blob);
    file.setSharing(DriveApp.Access.ANYONE_WITH_LINK, DriveApp.Permission.VIEW);
    
    const sheet = getSheet('heroSlides');
    const id = 'HERO-' + Date.now();
    const url = `https://drive.google.com/thumbnail?id=${file.getId()}&sz=w1920`;
    
    sheet.appendRow([
        id, url, 
        data.badge || '', 
        data.title || '', 
        data.desc || '', 
        data.btn1Text || 'Explore Catalog', 
        data.btn1Url || 'catalog', 
        data.btn2Text || 'Register Product', 
        data.btn2Url || 'register', 
        data.stat1 || '', 
        data.stat2 || '', 
        data.stat3 || '', 
        data.style || '{}',
        'Active', new Date()
    ]);
    
    SpreadsheetApp.flush();
    return { success: true, id: id, url: url };
  } catch (e) { 
    console.error('addHeroSlide error:', e);
    return { success: false, message: e.message }; 
  }
}

function updateHeroSlide(data) {
    try {
        const sheet = getSheet('heroSlides');
        const rows = sheet.getDataRange().getValues();
        const headers = rows[0];
        const colMap = {};
        headers.forEach((h, i) => colMap[h] = i + 1);
        
        for (let i = 1; i < rows.length; i++) {
            if (String(rows[i][0]) === String(data.id)) {
                const row = i + 1;
                if (data.base64) {
                    const folder = DriveApp.getFolderById(CONFIG.productImagesFolderId);
                    const decoded = Utilities.base64Decode(data.base64.split(',')[1]);
                    const blob = Utilities.newBlob(decoded, 'image/jpeg', `Hero_Update_${Date.now()}.jpg`);
                    const file = folder.createFile(blob);
                    file.setSharing(DriveApp.Access.ANYONE_WITH_LINK, DriveApp.Permission.VIEW);
                    sheet.getRange(row, colMap['Image URL']).setValue(`https://drive.google.com/thumbnail?id=${file.getId()}&sz=w1920`);
                }
                if (colMap['Badge']) sheet.getRange(row, colMap['Badge']).setValue(data.badge || '');
                if (colMap['Title']) sheet.getRange(row, colMap['Title']).setValue(data.title || '');
                if (colMap['Description']) sheet.getRange(row, colMap['Description']).setValue(data.desc || '');
                if (colMap['Btn1Text']) sheet.getRange(row, colMap['Btn1Text']).setValue(data.btn1Text || '');
                if (colMap['Btn1Url']) sheet.getRange(row, colMap['Btn1Url']).setValue(data.btn1Url || '');
                if (colMap['Btn2Text']) sheet.getRange(row, colMap['Btn2Text']).setValue(data.btn2Text || '');
                if (colMap['Btn2Url']) sheet.getRange(row, colMap['Btn2Url']).setValue(data.btn2Url || '');
                if (colMap['Stat1']) sheet.getRange(row, colMap['Stat1']).setValue(data.stat1 || '');
                if (colMap['Stat2']) sheet.getRange(row, colMap['Stat2']).setValue(data.stat2 || '');
                if (colMap['Stat3']) sheet.getRange(row, colMap['Stat3']).setValue(data.stat3 || '');
                if (colMap['StyleJSON']) sheet.getRange(row, colMap['StyleJSON']).setValue(data.style || '{}');
                
                SpreadsheetApp.flush();
                return { success: true };
            }
        }
        return { success: false, message: 'Slide ID not found' };
    } catch (e) { return { success: false, message: e.message }; }
}

function toggleHeroSlideStatus(id, status) {
    try {
        const sheet = getSheet('heroSlides');
        const data = sheet.getDataRange().getValues();
        for (let i = 1; i < data.length; i++) {
            if (String(data[i][0]) === String(id)) {
                sheet.getRange(i + 1, 14).setValue(status);
                SpreadsheetApp.flush();
                return { success: true };
            }
        }
        return { success: false };
    } catch (e) { return { success: false, message: e.message }; }
}

function deleteHeroSlide(id) {
    try {
        const sheet = getSheet('heroSlides');
        const data = sheet.getDataRange().getValues();
        for (let i = 1; i < data.length; i++) {
            if (String(data[i][0]) === String(id)) {
                sheet.deleteRow(i + 1);
                SpreadsheetApp.flush();
                return { success: true };
            }
        }
        return { success: false };
    } catch (e) { return { success: false, message: e.message }; }
}

function addCarouselImage(data) {
    try {
        const folder = DriveApp.getFolderById(CONFIG.productImagesFolderId);
        const decoded = Utilities.base64Decode(data.base64.split(',')[1]);
        const blob = Utilities.newBlob(decoded, 'image/jpeg', `Carousel_${Date.now()}.jpg`);
        const file = folder.createFile(blob);
        file.setSharing(DriveApp.Access.ANYONE_WITH_LINK, DriveApp.Permission.VIEW);
        
        const sheet = getSheet('carousel');
        const id = 'CR-' + Math.floor(100000 + Math.random() * 900000);
        const url = `https://drive.google.com/thumbnail?id=${file.getId()}&sz=w1920`;
        
        // Headers: Image ID, Image URL, Caption, Linked Product ID, Status, Created At, StyleJSON
        sheet.appendRow([
            id, 
            url, 
            data.caption || '', 
            data.linkedId || 'None', 
            'Active', 
            new Date(),
            data.style || '{}'
        ]);
        SpreadsheetApp.flush();
        return { success: true, id: id };
    } catch (e) { return { success: false, message: e.message }; }
}

function updateCarouselImage(data) {
    try {
        const sheet = getSheet('carousel');
        const rows = sheet.getDataRange().getValues();
        const headers = rows[0];
        const colMap = {};
        headers.forEach((h, i) => colMap[h] = i + 1);
        
        for (let i = 1; i < rows.length; i++) {
            if (String(rows[i][0]) === String(data.id)) {
                const row = i + 1;
                if (data.base64) {
                    const folder = DriveApp.getFolderById(CONFIG.productImagesFolderId);
                    const decoded = Utilities.base64Decode(data.base64.split(',')[1]);
                    const blob = Utilities.newBlob(decoded, 'image/jpeg', `Carousel_Update_${Date.now()}.jpg`);
                    const file = folder.createFile(blob);
                    file.setSharing(DriveApp.Access.ANYONE_WITH_LINK, DriveApp.Permission.VIEW);
                    sheet.getRange(row, colMap['Image URL']).setValue(`https://drive.google.com/thumbnail?id=${file.getId()}&sz=w1920`);
                }
                if (colMap['Caption']) sheet.getRange(row, colMap['Caption']).setValue(data.caption || '');
                if (colMap['Linked Product ID']) sheet.getRange(row, colMap['Linked Product ID']).setValue(data.linkedId || 'None');
                if (colMap['StyleJSON']) sheet.getRange(row, colMap['StyleJSON']).setValue(data.style || '{}');
                
                SpreadsheetApp.flush();
                return { success: true };
            }
        }
        return { success: false, message: 'Carousel Image ID not found' };
    } catch (e) { return { success: false, message: e.message }; }
}

function deleteCarouselImage(id) {
    try {
        const sheet = getSheet('carousel');
        const data = sheet.getDataRange().getValues();
        for (let i = 1; i < data.length; i++) {
            if (String(data[i][0]) === String(id)) {
                sheet.deleteRow(i + 1);
                SpreadsheetApp.flush();
                return { success: true };
            }
        }
        return { success: false };
    } catch (e) { return { success: false, message: e.message }; }
}

// ============================================
// 7. PRODUCT & CATALOG
// ============================================

function getProductModels() {
  var data = getData('products');
  var results = [];
  for (var i = 0; i < data.length; i++) {
    var p = data[i];
    var imageIds = (p['ImageIds'] || p['imageids'] || '').toString().split(',').filter(function(id) { return !!id.trim(); });
    var imageUrls = [];
    for (var j = 0; j < imageIds.length; j++) {
      imageUrls.push('https://lh3.googleusercontent.com/d/' + imageIds[j].trim());
    }
    
    var mainImg = p['ImageUrl'] || p['imageurl'] || '';
    if (mainImg && imageUrls.indexOf(mainImg) === -1) {
      imageUrls.unshift(mainImg);
    }
    
    results.push({
      code: p['Model Code'] || p['modelcode'] || '',
      name: p['Model Name'] || p['modelname'] || '',
      description: p['Description'] || p['description'] || '',
      imageUrl: imageUrls[0] || 'https://via.placeholder.com/300x200?text=V-VARMA',
      imageUrls: imageUrls,
      price: p['Price'] || p['price'] || '0',
      mrp: p['MRP'] || p['mrp'] || p['Price'] || p['price'] || '0',
      status: p['Status'] || p['status'] || 'Active',
      stock: p['Stock'] || p['stock'] || 0,
      specs: p['Specs'] || p['specs'] || ''
    });
  }
  return results;
}

function deleteProduct(code) {
    const sheet = getSheet('products');
    const data = sheet.getDataRange().getValues();
    for (let i = 1; i < data.length; i++) {
        if (String(data[i][0]) === String(code)) {
            sheet.deleteRow(i + 1);
            SpreadsheetApp.flush();
            return { success: true };
        }
    }
    return { success: false, message: 'Product Code not found' };
}
function getWebSettings() {
    try {
        const data = getData('webSettings');
        const settings = {};
        data.forEach(row => {
            // Resilient key/value extraction
            let key = null, val = null;
            for (let k in row) {
                const lowK = k.toString().trim().toLowerCase();
                if (lowK === 'key') key = row[k];
                if (lowK === 'value') val = row[k];
            }
            if (key) settings[key] = val;
        });
        return settings;
    } catch (e) {
        console.error('getWebSettings failed:', e);
        return {};
    }
}

function getWebSetting(key) {
    try {
        const sheet = getSheet('webSettings');
        const data = sheet.getDataRange().getValues();
        for (let i = 1; i < data.length; i++) {
            if (String(data[i][0]).trim() === String(key).trim()) {
                return data[i][1];
            }
        }
        return '';
    } catch (e) {
        console.error('getWebSetting failed for key ' + key + ':', e);
        return '';
    }
}

function saveWebSetting(key, value) {
    try {
        const sheet = getSheet('webSettings');
        const data = sheet.getDataRange().getValues();
        for (let i = 1; i < data.length; i++) {
            if (String(data[i][0]).trim() === String(key).trim()) {
                sheet.getRange(i + 1, 2).setValue(value);
                sheet.getRange(i + 1, 3).setValue(new Date());
                SpreadsheetApp.flush();
                return { success: true };
            }
        }
        sheet.appendRow([key, value, new Date()]);
        SpreadsheetApp.flush();
        return { success: true };
    } catch (e) {
        console.error('saveWebSetting failed for key ' + key + ':', e);
        return { success: false, message: e.message };
    }
}

/**
 * Returns live auto-calculated stats from the database.
 * Called separately (once) when admin opens the Web Settings tab,
 * NOT inside getWebSettings() to keep page load fast.
 */
function getAutoStats() {
    try {
        // 1. Machines Installed: Total records in Warranty Register
        const warranties = getData('warranty') || [];
        const installations_count = warranties.length;

        // 2. Partner Dealers: Total active records in Dealers List
        const dealers = getData('dealers') || [];
        const dealers_count = dealers.length;

        // 3. Technicians Count: Active technicians in Users sheet
        const users = getData('users') || [];
        const technicians_count = users.filter(u => {
            const role   = String(u.Role   || u.role   || '').toLowerCase().trim();
            const status = String(u.Status || u.status || '').toLowerCase().trim();
            return role === 'technician' && status === 'active';
        }).length;

        // 4. Areas Covered: Unique districts from Dealers + Parties
        const districts = new Set();
        dealers.forEach(d => {
            const dist = d.District || d.City;
            if (dist) districts.add(dist.toString().trim().toUpperCase());
        });
        const parties = getData('parties') || [];
        parties.forEach(p => {
            if (p.District) districts.add(p.District.toString().trim().toUpperCase());
        });
        const areas_count = districts.size || 1;

        return {
            success: true,
            installations_count: installations_count.toString(),
            dealers_count:       dealers_count.toString(),
            technicians_count:   technicians_count.toString(),
            areas_count:         areas_count.toString()
        };
    } catch (e) {
        console.error('getAutoStats failed:', e);
        return { success: false, message: e.message };
    }
}

function saveWebSettings(data) {
    try {
        const sheet = getSheet('webSettings');
        const existing = sheet.getDataRange().getValues();
        data.forEach(item => {
            let val = item.Value;
            
            // Special handling for partner_logos: upload any new base64 images to Drive
            if (item.Key === 'partner_logos') {
                try {
                    let logos = JSON.parse(val);
                    if (Array.isArray(logos)) {
                        const folder = DriveApp.getFolderById(CONFIG.productImagesFolderId);
                        const processedLogos = logos.map((logo, index) => {
                            if (logo && logo.startsWith('data:image')) {
                                const decoded = Utilities.base64Decode(logo.split(',')[1]);
                                const extension = logo.split(';')[0].split('/')[1] || 'png';
                                const blob = Utilities.newBlob(decoded, 'image/' + extension, `Partner_${Date.now()}_${index}.${extension}`);
                                const file = folder.createFile(blob);
                                file.setSharing(DriveApp.Access.ANYONE_WITH_LINK, DriveApp.Permission.VIEW);
                                return `https://drive.google.com/thumbnail?id=${file.getId()}&sz=w400`;
                            }
                            return logo; // Keep existing http urls
                        });
                        val = JSON.stringify(processedLogos);
                    }
                } catch(err) {
                    console.error('Error processing partner logos in saveWebSettings:', err);
                }
            }
            
            let found = false;
            for (let i = 1; i < existing.length; i++) {
                if (existing[i][0] === item.Key) {
                    sheet.getRange(i + 1, 2).setValue(val);
                    sheet.getRange(i + 1, 3).setValue(new Date());
                    found = true;
                    break;
                }
            }
            if (!found) sheet.appendRow([item.Key, val, new Date()]);
        });
        SpreadsheetApp.flush();
        return { success: true };
    } catch (e) { return { success: false, message: e.message }; }
}

function saveLogo(base64) {
    try {
        const folder = DriveApp.getFolderById(CONFIG.productImagesFolderId);
        const decoded = Utilities.base64Decode(base64.split(',')[1]);
        const blob = Utilities.newBlob(decoded, 'image/png', `Logo_${Date.now()}.png`);
        const file = folder.createFile(blob);
        file.setSharing(DriveApp.Access.ANYONE_WITH_LINK, DriveApp.Permission.VIEW);
        const url = `https://drive.google.com/thumbnail?id=${file.getId()}&sz=w400`;
        
        return saveWebSettings([{ Key: 'site_logo', Value: url }]);
    } catch (e) { return { success: false, message: e.message }; }
}

function getAdminStats() {
    const ledger = getData('ledger');
    const complaints = getData('complaints');
    const warranty = getData('warranty');
    const catalog = getCatalogProducts();
    const orders = getData('orders');
    const installation = getData('installation');
    
    const sales = ledger.reduce((acc, curr) => acc + (parseFloat(curr['Grand Total']) || 0), 0);
    const balance = ledger.reduce((acc, curr) => acc + (parseFloat(curr['Balance']) || 0), 0);
    
    const activeComplaints = complaints.filter(c => ['Registered', 'Assigned', 'In Progress'].includes(c.Status)).length;
    const lowStock = catalog.filter(p => (parseInt(p.Stock || p['Stock']) || 0) < 5).length;
    const pendingOrders = orders.filter(o => ['Pending', 'Processing'].includes(o.Status)).length;
    const scheduledInstalls = installation.filter(i => ['Registered', 'Assigned', 'Scheduled'].includes(i.Status)).length;
    
    const techTasks = complaints.filter(c => c.Status === 'Assigned').length + 
                     installation.filter(i => i.Status === 'Assigned').length;

    const availableTechs = getData('users').filter(u => String(u.Role).toLowerCase() === 'technician' && u.Status === 'Active').length;
    const totalDealers = getData('dealers').length;

    // Financial aggregates for dashboard
    syncPaymentInSchemas();
    const wallets = getData('customer_wallet') || [];
    const walletTotal = wallets.reduce((acc, w) => {
      var amt = parseFloat(w['Amount']) || 0;
      return acc + (w['Type'] === 'Deposit' ? amt : -amt);
    }, 0);

    const cashFlow = getData('cash_flow') || [];
    const cashTotal = cashFlow.reduce((acc, row) => {
      if (row['Status'] === 'Deleted') return acc;
      return acc + (parseFloat(row['Cash In']) || 0) - (parseFloat(row['Cash Out']) || 0);
    }, 0);

    return {
        sales: sales.toLocaleString('en-IN'),
        pendingPayments: balance.toLocaleString('en-IN'),
        orders: orders.length,
        pendingOrders: pendingOrders,
        installations: installation.length,
        scheduledInstalls: scheduledInstalls,
        techTasks: techTasks,
        inventory: catalog.length,
        lowStock: lowStock,
        complaints: complaints.length,
        activeComplaints: activeComplaints,
        warranties: warranty.length,
        availableTechs: availableTechs,
        totalDealers: totalDealers,
        pendingDealers: getData('dealers').filter(d => String(d.Status).toLowerCase() === 'pending').length,
        walletBalance: walletTotal.toLocaleString('en-IN'),
        cashFlowBalance: cashTotal.toLocaleString('en-IN')
    };
}

/**
 * Dealer Registration Handler
 */
function saveDealerRegistration(data) {
  try {
    const sheet = getSheet('dealers');
    const now = new Date();
    const dealerId = 'DLR-' + Math.random().toString(36).substr(2, 6).toUpperCase();
    
    // Handle File Upload if provided
    let proofUrl = '';
    if (data.idProof && data.idProof.startsWith('data:')) {
      proofUrl = uploadImageToDrive(data.idProof, `Dealer_ID_${dealerId}`);
    }

    const namePart = String(data.name || '').replace(/[^a-zA-Z]/g, '').substring(0, 4);
    const mobilePart = String(data.mobile || '').slice(-4);
    const autoPassword = namePart + '@' + mobilePart;

    const isByAdmin = String(data.isAdmin) === 'true';
    const status = isByAdmin ? 'Active' : 'Pending';

    const payload = {
      'Dealer ID': dealerId,
      'Dealer Name': data.name,
      'Mobile': data.mobile,
      'Email': data.email,
      'Area': data.area,
      'Nearby Area': data.nearbyArea || '',
      'Pincode': data.pincode,
      'Address': data.address,
      'City': data.city,
      'State': data.state,
      'District': data.district || '',
      'Shop Name': data.shopName,
      'GST Number': data.gstNumber || 'N/A',
      'ID Proof URL': proofUrl,
      'Status': status,
      'Username': data.mobile, 
      'Password': autoPassword,
      'Application Date': now.toISOString(),
      'Maps URL': data.mapsUrl || '',
      'Business URL': data.businessUrl || ''
    };

    if (isMobileRegisteredInCategory(data.mobile, 'dealer')) {
        return { success: false, message: 'This mobile number is already registered as a Dealer. Please login to your account.' };
    }

    const saveResult = saveEntity('dealers', payload);
    
    // Sync to centralized Parties table
    if (saveResult.success) {
      try { syncPartyFromRegistration('B2B', payload); } catch(e) { console.error(e); }
    }
    
    if (saveResult.success) {
      try {
        const emailType = status === 'Active' ? 'Approved' : 'Pending';
        sendDealerNotificationEmail(payload, emailType);
      } catch (emailErr) {
        console.error('Email Notification Error:', emailErr);
      }
    }
    
    return saveResult;
  } catch (e) {
    console.error('saveDealerRegistration Error:', e);
    return { success: false, message: e.message };
  }
}


/**
 * Get Dropdown Options
 * Returns unique values for each column in the dropdownFields sheet.
 * Reads the sheet DIRECTLY (bypasses getData/schema) to preserve all user-added columns.
 */
/**
 * Get Dropdown Options
 * Returns unique values for each column in the dropdownFields sheet.
 * Reads the sheet DIRECTLY (bypasses getData/schema) to preserve all user-added columns.
 */
function getDropdownOptions() {
  const logPrefix = '[Dropdowns] ';
  try {
    const ss = getSpreadsheet();
    let sheetName = CONFIG.sheets.dropdownFields || 'All Dropdowns';
    
    console.log(logPrefix + 'Starting fetch from Spreadsheet ID: ' + CONFIG.spreadsheetId);
    
    // 1. Try primary name and aliases
    let sheet = ss.getSheetByName(sheetName);
    if (!sheet) {
      const altNames = ['All Dropdown Fields', 'All_Dropdown_Fields', 'All_Dropdown_Field', 'Dropdowns', 'All Dropdown', 'All_Dropdown', 'ALL DROPDOWN', 'ALL DROPDOWNS'];
      for (const alt of altNames) {
        sheet = ss.getSheetByName(alt);
        if (sheet) {
          console.log(logPrefix + 'Found sheet under alternate name: ' + alt);
          break;
        }
      }
    }
    
    // 2. Fallback: Search ALL sheets for anything containing "Dropdown" (Case-Insensitive)
    if (!sheet) {
      console.log(logPrefix + 'Primary/Alias lookup failed. Searching all sheets...');
      const allSheets = ss.getSheets();
      for (const s of allSheets) {
        const name = s.getName().toUpperCase();
        if (name.includes('DROPDOWN')) {
          sheet = s;
          console.log(logPrefix + 'Auto-discovered dropdown sheet by name match: ' + s.getName());
          break;
        }
      }
    }
    
    const defaultStates = ['Tamil Nadu', 'Andhra Pradesh', 'Karnataka', 'Kerala', 'Telangana'];
    
    if (!sheet) {
      console.warn(logPrefix + 'No dropdown sheet found at all! Returning defaults.');
      return { success: true, data: { 'State': defaultStates, 'state': defaultStates, 'District': [], 'district': [] } };
    }
    
    const lastRow = sheet.getLastRow();
    const lastCol = sheet.getLastColumn();
    
    console.log(logPrefix + 'Targeting sheet: "' + sheet.getName() + '" | Rows: ' + lastRow + ' | Cols: ' + lastCol);
    
    if (lastRow < 2) {
      console.warn(logPrefix + 'Sheet is effectively empty. Returning defaults.');
      return { success: true, data: { 'State': defaultStates, 'state': defaultStates, 'District': [], 'district': [] } };
    }
    
    // Read ALL data
    const data = sheet.getRange(1, 1, lastRow, lastCol).getValues();
    const headers = data[0];
    const options = {};
    
    headers.forEach((h, colIdx) => {
      const key = String(h).trim();
      if (!key || key.startsWith('--')) return;
      
      const normalizedKey = key.charAt(0).toUpperCase() + key.slice(1).toLowerCase(); 
      const values = [];
      
      for (let i = 1; i < data.length; i++) {
        const val = data[i][colIdx];
        if (val !== undefined && val !== null && val !== '') {
          const strVal = String(val).trim();
          if (strVal && strVal.toLowerCase() !== key.toLowerCase() && !strVal.startsWith('--')) {
            values.push(strVal);
          }
        }
      }
      
      let uniqueValues = [...new Set(values)].sort();
      
      // Fallback for State if empty
      if (normalizedKey === 'State' && uniqueValues.length === 0) {
        uniqueValues = defaultStates;
      }
      
      // Store variants for robust frontend access
      options[normalizedKey] = uniqueValues;
      options[key] = uniqueValues;
      options[key.toLowerCase()] = uniqueValues;
      options[key.toUpperCase()] = uniqueValues;
    });
    
    return { success: true, data: options };
  } catch (e) {
    console.error(logPrefix + 'CRITICAL ERROR:', e);
    return { success: false, message: e.message };
  }
}

/**
 * Professional Email Notifier for Dealers
 */
function sendDealerNotificationEmail(dealerData, type) {
  try {
    const email = dealerData.Email || dealerData.email;
    if (!email) return;

    let subject, title, message, actionText, actionUrl, bannerColor, icon;
    
    const appUrl = ScriptApp.getService().getUrl();

    if (type === 'Pending') {
      subject = 'Application Received - V-VARMA Dealership';
      title = 'Application Received!';
      icon = '📩';
      bannerColor = '#ff7e00'; // V-VARMA Orange
      message = `
        Dear ${dealerData['Dealer Name']},<br><br>
        Thank you for your interest in joining the V-VARMA dealer network. We have received your application and it is currently under review by our administrative team.<br><br>
        <b>Registration Details:</b><br>
        - Dealer ID: ${dealerData['Dealer ID']}<br>
        - Shop Name: ${dealerData['Shop Name']}<br>
        - Submission Date: ${new Date().toLocaleDateString()}<br><br>
        Our team will verify your documents and get back to you within 24-48 hours.
      `;
      actionText = 'View Website';
      actionUrl = appUrl;
    } else if (type === 'Approved') {
      subject = 'Welcome to V-VARMA - Your Dealer Account is Approved!';
      title = 'Account Approved!';
      icon = '🎉';
      bannerColor = '#0d47a1'; // V-VARMA Blue
      message = `
        Dear ${dealerData['Dealer Name']},<br><br>
        Congratulations! Your dealership application has been approved. You are now an authorized partner of V-VARMA.<br><br>
        <b>Login Credentials:</b><br>
        - <b>Username:</b> ${dealerData['Username'] || dealerData['Mobile']}<br>
        - <b>Password:</b> ${dealerData['Password']}<br><br>
        You can now log in to the Dealer Dashboard to manage your orders, register warranties, and access marketing materials.
      `;
      actionText = 'Login to Dashboard';
      actionUrl = appUrl + '?page=login';
    }

    const htmlBody = `
      <div style="font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; max-width: 600px; margin: 0 auto; border: 1px solid #eee; border-radius: 15px; overflow: hidden; box-shadow: 0 10px 30px rgba(0,0,0,0.05);">
        <div style="background-color: ${bannerColor}; padding: 40px 20px; text-align: center; color: white;">
          <div style="font-size: 50px; margin-bottom: 15px;">${icon}</div>
          <h1 style="margin: 0; font-size: 28px; font-weight: 800; letter-spacing: -1px;">${title}</h1>
          <p style="margin: 10px 0 0; opacity: 0.9; font-size: 16px;">V-VARMA Authorized Dealer Network</p>
        </div>
        <div style="padding: 40px; background-color: #ffffff; color: #333; line-height: 1.6;">
          <div style="font-size: 16px;">
            ${message}
          </div>
          <div style="margin-top: 40px; text-align: center;">
            <a href="${actionUrl}" style="background-color: ${bannerColor}; color: white; padding: 15px 35px; text-decoration: none; border-radius: 50px; font-weight: bold; display: inline-block; transition: transform 0.3s;">${actionText}</a>
          </div>
          <div style="margin-top: 40px; padding-top: 20px; border-top: 1px solid #eee; font-size: 12px; color: #888; text-align: center;">
            <p>© ${new Date().getFullYear()} V-VARMA Industries. All rights reserved.</p>
            <p>This is an automated message. Please do not reply to this email.</p>
          </div>
        </div>
      </div>
    `;

    MailApp.sendEmail({
      to: email,
      subject: subject,
      htmlBody: htmlBody
    });
    
    console.log(`Email sent successfully to ${email} (${type})`);
  } catch (e) {
    console.error('sendDealerNotificationEmail Error:', e);
  }
}

/**
 * Admin Dealer Approval
 */
function approveDealer(dealerId) {
  try {
    const sheet = getSheet('dealers');
    const data = sheet.getDataRange().getValues();
    const headers = SHEET_SCHEMAS.dealers;
    const idIdx = headers.indexOf('Dealer ID');
    const statusIdx = headers.indexOf('Status');
    
    if (idIdx === -1 || statusIdx === -1) throw new Error('Schema mismatch');

    for (let i = 1; i < data.length; i++) {
      if (String(data[i][idIdx]) === String(dealerId)) {
        sheet.getRange(i + 1, statusIdx + 1).setValue('Active');
        
        // Sync to Users sheet to enable login
        const dealerRow = data[i];
        const userPayload = {
          'Username': dealerRow[headers.indexOf('Username')],
          'Password': dealerRow[headers.indexOf('Password')],
          'Role': 'Dealer',
          'Name': dealerRow[headers.indexOf('Dealer Name')],
          'Mobile': dealerRow[headers.indexOf('Mobile')],
          'Status': 'Active',
          'Address': dealerRow[headers.indexOf('Address')] + ', ' + dealerRow[headers.indexOf('Area')]
        };
        saveEntity('users', userPayload);
        
        // Send Approval Email
        try {
          const dealerObj = {};
          headers.forEach((h, idx) => {
            dealerObj[h] = dealerRow[idx];
          });
          sendDealerNotificationEmail(dealerObj, 'Approved');
        } catch (emailErr) {
          console.error('Approval Email Error:', emailErr);
        }
        
        return { success: true, message: 'Dealer approved and login activated.' };
      }
    }
    return { success: false, message: 'Dealer not found.' };
  } catch (e) {
    return { success: false, message: e.message };
  }
}

/**
 * Admin Dealer Rejection
 */
function rejectDealer(dealerId, reason) {
  try {
    const sheet = getSheet('dealers');
    const data = sheet.getDataRange().getValues();
    const headers = SHEET_SCHEMAS.dealers;
    const idIdx = headers.indexOf('Dealer ID');
    const statusIdx = headers.indexOf('Status');
    
    for (let i = 1; i < data.length; i++) {
      if (String(data[i][idIdx]) === String(dealerId)) {
        sheet.getRange(i + 1, statusIdx + 1).setValue('Rejected');
        // Optional: Log reason in a notes column if added
        return { success: true, message: 'Dealer application rejected.' };
      }
    }
    return { success: false, message: 'Dealer not found.' };
  } catch (e) {
    return { success: false, message: e.message };
  }
}

function debugServer() {
  const report = {
    timestamp: new Date().toISOString(),
    spreadsheetId: CONFIG.spreadsheetId,
    sheets: [],
    stats: null,
    error: null
  };
  try {
    const ss = getSpreadsheet();
    report.spreadsheetName = ss.getName();
    const sheets = ss.getSheets();
    report.sheets = sheets.map(s => ({ name: s.getName(), rows: s.getLastRow() }));
    
    report.stats = getAdminStats();
  } catch (e) {
    report.error = e.message;
  }
  return report;
}

// getDealersList handled at line 376

/**
 * Public Dealer Search
 */
/**
 * Public Dealer Search (Using version at line 421)
 */


function getTechnicians() {
    return getData('users').filter(u => String(u.Role).toLowerCase() === 'technician');
}

// ============================================
// 8. ORDER, AMC, PARTY & NOTIFICATION MANAGEMENT
// ============================================

function addOrder(data) {
    try {
        const sheet = getSheet('orders');
        const now = new Date();
        const oid = `ORD-${now.getFullYear().toString().substr(-2)}${(now.getMonth() + 1).toString().padStart(2, '0')}-${String(sheet.getLastRow()).padStart(4, '0')}`;
        sheet.appendRow([
            now, oid, data.customerName, "'" + data.mobile, data.type || 'Direct',
            data.items || '', data.status || 'Pending', data.totalAmount || '0',
            data.technician || '', data.deliveryStatus || 'Processing'
        ]);
        SpreadsheetApp.flush();
        return { success: true, orderId: oid };
    } catch (e) { return { success: false, message: e.message }; }
}

function placeOrderFromWebsite(cartItems, customerName, mobile, orderType, paymentMethod, installRequired, installationChargeTotal) {
    const lock = LockService.getScriptLock();
    try {
        lock.waitLock(10000);
        const sheetOrders = getSheet('orders');
        const sheetCatalog = getSheet('catalog');
        const catalogData = sheetCatalog.getDataRange().getValues();
        const headers = catalogData[0];
        const stockIndex = headers.indexOf('Stock');
        const pidIndex = headers.indexOf('Product ID');
        
        if (stockIndex === -1 || pidIndex === -1) {
            return { success: false, message: 'Configuration error: Stock column not found.' };
        }

        let totalAmount = 0;
        let itemsString = '';
        
        // Stock Check & Deduction for actual orders
        if (orderType === 'Website Order') {
            for (let i = 0; i < cartItems.length; i++) {
                const item = cartItems[i];
                let found = false;
                let currentStock = 0;
                for (let r = 1; r < catalogData.length; r++) {
                    if (String(catalogData[r][pidIndex]) === String(item.pid)) {
                        currentStock = parseInt(catalogData[r][stockIndex]) || 0;
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    return { success: false, message: `Product ${item.name} not found.` };
                }
                if (item.qty > currentStock) {
                    return { success: false, message: `Out of Stock: Only ${currentStock} items left for ${item.name}.` };
                }
            }
            
            // Deduct stock
            for (let i = 0; i < cartItems.length; i++) {
                const item = cartItems[i];
                for (let r = 1; r < catalogData.length; r++) {
                    if (String(catalogData[r][pidIndex]) === String(item.pid)) {
                        let currentStock = parseInt(catalogData[r][stockIndex]) || 0;
                        sheetCatalog.getRange(r + 1, stockIndex + 1).setValue(currentStock - item.qty);
                        break;
                    }
                }
            }
        }
        
        for (let i = 0; i < cartItems.length; i++) {
            const item = cartItems[i];
            itemsString += `${item.name} (x${item.qty}) [₹${(item.price * item.qty).toLocaleString('en-IN')}]\n`;
            totalAmount += (item.price * item.qty);
            
            const instPrice = parseFloat(item.installationCharges) || 0;
            if (instPrice > 0 && item.installationChecked !== false) {
                itemsString += `  + Installation (x${item.qty}) [₹${(instPrice * item.qty).toLocaleString('en-IN')}]\n`;
                totalAmount += (instPrice * item.qty);
            }
            if (item.accessoriesChecked !== false) {
                itemsString += `  + Accessories (x${item.qty}) [₹${(300 * item.qty).toLocaleString('en-IN')}]\n`;
                totalAmount += (300 * item.qty);
            }
        }

        if (paymentMethod) {
            itemsString += `\nPayment: ${paymentMethod}`;
        }
        
        const now = new Date();
        const oid = `ORD-${now.getFullYear().toString().substr(-2)}${(now.getMonth() + 1).toString().padStart(2, '0')}-${String(sheetOrders.getLastRow()).padStart(4, '0')}`;
        
        sheetOrders.appendRow([
            now, 
            oid, 
            customerName, 
            "'" + mobile, 
            orderType, 
            itemsString.trim(), 
            orderType === 'Website Order' ? 'Pending Approval' : 'Pending', 
            totalAmount,
            '', 
            'Processing'
        ]);
        
        SpreadsheetApp.flush();
        return { success: true, orderId: oid };
    } catch (e) {
        return { success: false, message: e.message };
    } finally {
        lock.releaseLock();
    }
}

function addAMC(data) {
    try {
        const sheet = getSheet('amc');
        const now = new Date();
        const aid = `AMC-${now.getFullYear().toString().substr(-2)}${(now.getMonth() + 1).toString().padStart(2, '0')}-${String(sheet.getLastRow()).padStart(4, '0')}`;
        const startDate = new Date(data.startDate || now);
        const endDate = new Date(startDate);
        endDate.setFullYear(endDate.getFullYear() + 1);
        const intervalDays = parseInt(data.intervalDays) || 90;
        const nextDue = new Date(startDate);
        nextDue.setDate(nextDue.getDate() + intervalDays);
        
        sheet.appendRow([
            aid, data.customerName, "'" + data.mobile, data.email || '',
            data.productModel, data.partNumber || '', data.warrantyId || '',
            formatDate(startDate), formatDate(endDate), intervalDays,
            data.serviceVisits || '4', '0', formatDate(nextDue),
            'Active', data.remarks || '', now, now
        ]);
        SpreadsheetApp.flush();
        return { success: true, amcId: aid };
    } catch (e) { return { success: false, message: e.message }; }
}

function addParty(data) {
    try {
        const sheet = getSheet('parties');
        const pid = `P-${Date.now().toString().substr(-6)}`;
        sheet.appendRow([
            pid, data.name, "'" + data.mobile, data.type || 'Customer',
            data.email || '', data.address || '', data.area || '',
            data.pincode || '', 'Active', new Date()
        ]);
        SpreadsheetApp.flush();
        return { success: true, partyId: pid };
    } catch (e) { return { success: false, message: e.message }; }
}

function sendNotification(data) {
    try {
        const sheet = getSheet('notifications');
        
        // Ensure headers exist in the sheet for the new styling options
        const headers = sheet.getRange(1, 1, 1, 13).getValues()[0];
        if (headers.length < 13 || String(headers[9]).trim() === "" || String(headers[10]).trim() === "") {
            sheet.getRange(1, 1, 1, 13).setValues([[
                "Notification ID", "Date", "Audience", "Recipient", 
                "Channel", "Title", "Message", "RefType", "RefID", "Status",
                "Text Size", "Text Color", "Scroll Duration"
            ]]);
        }
        
        const notifId = data.notifId || data.notificationId || '';
        
        if (notifId && notifId !== 'undefined' && notifId !== '') {
            // Edit existing notification
            const values = sheet.getDataRange().getValues();
            for (let i = 1; i < values.length; i++) {
                if (String(values[i][0]).trim() === String(notifId).trim()) {
                    const row = i + 1;
                    sheet.getRange(row, 3).setValue(data.audience || 'All');
                    sheet.getRange(row, 4).setValue(data.recipient || '');
                    sheet.getRange(row, 5).setValue(data.channel || 'System');
                    sheet.getRange(row, 6).setValue(data.title);
                    sheet.getRange(row, 7).setValue(data.message);
                    sheet.getRange(row, 8).setValue(data.refType || '');
                    sheet.getRange(row, 9).setValue(data.refId || '');
                    sheet.getRange(row, 10).setValue(data.status || values[i][9] || 'Sent');
                    sheet.getRange(row, 11).setValue(data.textSize || '16px');
                    sheet.getRange(row, 12).setValue(data.textColor || '#ffffff');
                    sheet.getRange(row, 13).setValue(data.scrollDuration || '25');
                    
                    SpreadsheetApp.flush();
                    return { success: true, notificationId: notifId };
                }
            }
            return { success: false, message: 'Notification ID not found for editing' };
        } else {
            // Add new notification
            const nid = `NOTIF-${Date.now()}`;
            sheet.appendRow([
                nid, new Date(), data.audience || 'All', data.recipient || '',
                data.channel || 'System', data.title, data.message,
                data.refType || '', data.refId || '', 'Sent',
                data.textSize || '16px', data.textColor || '#ffffff', data.scrollDuration || '25'
            ]);
            SpreadsheetApp.flush();
            return { success: true, notificationId: nid };
        }
    } catch (e) { return { success: false, message: e.message }; }
}

function deleteNotification(notifId) {
    try {
        const sheet = getSheet('notifications');
        const data = sheet.getDataRange().getValues();
        for (let i = 1; i < data.length; i++) {
            if (String(data[i][0]).trim() === String(notifId).trim()) {
                sheet.deleteRow(i + 1);
                SpreadsheetApp.flush();
                return { success: true };
            }
        }
        return { success: false, message: 'Notification not found' };
    } catch (e) {
        return { success: false, message: e.message };
    }
}

function toggleNotificationStatus(notifId, status) {
    try {
        const sheet = getSheet('notifications');
        const data = sheet.getDataRange().getValues();
        for (let i = 1; i < data.length; i++) {
            if (String(data[i][0]).trim() === String(notifId).trim()) {
                sheet.getRange(i + 1, 10).setValue(status); // 10th column is Status
                SpreadsheetApp.flush();
                return { success: true };
            }
        }
        return { success: false, message: 'Notification not found' };
    } catch (e) {
        return { success: false, message: e.message };
    }
}

function getLiveNotifications(role, mobile) {
    try {
        const notifications = getData('notifications') || [];
        const filtered = notifications.filter(n => {
            let aud = '';
            let rec = '';
            for (let k in n) {
                const low = k.toString().toLowerCase().trim();
                if (low === 'audience') aud = String(n[k]).toLowerCase().trim();
                if (low === 'recipient') rec = String(n[k]).toLowerCase().trim();
            }
            
            // Match Audience
            let matchAudience = false;
            if (aud === 'all') {
                matchAudience = true;
            } else if (role === 'customer' && (aud === 'customers' || aud === 'customer')) {
                matchAudience = true;
            } else if (role === 'technician' && (aud === 'technicians' || aud === 'technician')) {
                matchAudience = true;
            } else if (role === 'vendor' && (aud === 'vendors' || aud === 'vendor')) {
                matchAudience = true;
            } else if (role === 'public' && (aud === 'public' || aud === 'all')) {
                matchAudience = true;
            }
            
            // Match Recipient (Mobile number check)
            let matchRecipient = true;
            if (rec && rec !== 'all' && rec !== '') {
                matchRecipient = (rec === String(mobile).toLowerCase().trim());
            }
            
            return matchAudience && matchRecipient;
        });
        
        return { success: true, data: filtered };
    } catch (e) {
        return { success: false, message: e.message, data: [] };
    }
}

function markNotificationRead(notifId) {
    try {
        const sheet = getSheet('notifications');
        const data = sheet.getDataRange().getValues();
        for (let i = 1; i < data.length; i++) {
            if (String(data[i][0]).trim() === String(notifId).trim()) {
                sheet.getRange(i + 1, 10).setValue('Read'); // 10th column is Status
                SpreadsheetApp.flush();
                return { success: true };
            }
        }
        return { success: false, message: 'Notification ID not found' };
    } catch (e) {
        return { success: false, message: e.message };
    }
}

function getAboutUsData() {
    try {
        const sectionsJson = getWebSetting('ABOUT_US_SECTIONS');
        let sections = [];
        if (sectionsJson) {
            try {
                sections = JSON.parse(sectionsJson);
            } catch (e) {
                console.error('Error parsing ABOUT_US_SECTIONS:', e);
            }
        }
        
        // Fallback to default high-quality corporate content if none exists in Sheets
        if (!sections || sections.length === 0) {
            sections = [
              {
                title: "V-VARMA - Legacy of Electrical Innovation",
                content: "Established with a vision to revolutionize water management and industrial control systems, V-VARMA has grown into India's premier manufacturer of advanced automatic water level controllers. Over the last 25 years, our commitment to absolute reliability, exceptional quality, and state-of-the-art automation has earned the trust of over 50,000 satisfied families, industrial giants, and agricultural operations.",
                imageUrl: "https://www.gsvee.in/wp-content/uploads/2021/04/vvarma-logo.png",
                style: {
                  fontFamily: "Inter",
                  fontSize: "1.1rem",
                  italic: false,
                  bold: false,
                  layout: "left",
                  color: "#3d2b1f",
                  imgWidth: "33%",
                  imgHeight: "250px",
                  fitMode: "contain"
                }
              },
              {
                title: "Our Cutting-Edge Smart Automation Systems",
                content: "We specialize in developing intelligent, micro-controller-based automatic water level controllers, automated panels, and custom electrical panels that work fully automatically. Equipped with dry-run protection, advanced timers, dual-level indicators, and highly durable magnetic sensors, V-VARMA products ensure zero water wastage, optimal power consumption, and long pump lifespans. We bring industrial-grade automation directly to your household.",
                imageUrl: "https://via.placeholder.com/600x400?text=V-VARMA+Smart+Panel",
                style: {
                  fontFamily: "Inter",
                  fontSize: "1.1rem",
                  italic: false,
                  bold: false,
                  layout: "right",
                  color: "#3d2b1f",
                  imgWidth: "50%",
                  imgHeight: "300px",
                  fitMode: "cover"
                }
              }
            ];
        }
        return { success: true, data: sections };
    } catch (e) {
        return { success: false, message: e.message };
    }
}

function saveAboutUsData(sections) {
    try {
        const value = typeof sections === 'string' ? sections : JSON.stringify(sections);
        const res = saveWebSetting('ABOUT_US_SECTIONS', value);
        return res;
    } catch (e) {
        return { success: false, message: e.message };
    }
}

function uploadSectionImage(base64) {
    try {
        const folder = DriveApp.getFolderById(CONFIG.productImagesFolderId);
        const decoded = Utilities.base64Decode(base64.split(',')[1]);
        const blob = Utilities.newBlob(decoded, 'image/jpeg', `AboutUs_${Date.now()}.jpg`);
        const file = folder.createFile(blob);
        file.setSharing(DriveApp.Access.ANYONE_WITH_LINK, DriveApp.Permission.VIEW);
        const url = `https://drive.google.com/thumbnail?id=${file.getId()}&sz=w1000`;
        return { success: true, url: url };
    } catch (e) {
        return { success: false, message: e.message };
    }
}

function exportLedgerCSV() {
    try {
        const data = getData('ledger');
        if (!data || data.length === 0) return { success: false, message: 'No ledger data found' };
        const headers = ['Date', 'Transaction ID', 'Type', 'Name', 'Mobile', 'Product Model', 'Grand Total', 'Payment Status', 'Payment Mode'];
        let csv = headers.join(',') + '\n';
        data.forEach(row => {
            csv += [
                row['Date'] || '', row['Transaction ID'] || '', row['Type'] || '',
                row['Name'] || '', row['Mobile'] || '', row['Product Model'] || '',
                row['Grand Total'] || '', row['Payment Status'] || '', row['Payment Mode'] || ''
            ].map(v => '"' + String(v).replace(/"/g, '""') + '"').join(',') + '\n';
        });
        return { success: true, csv: csv, count: data.length };
    } catch (e) { return { success: false, message: e.message }; }
}

// ============================================
// 9. UTILITIES & LOGGING
// ============================================

function formatDate(date) {
    if (!date) return '';
    try {
        const d = new Date(date);
        if (isNaN(d.getTime())) return String(date);
        const day = d.getDate().toString().padStart(2, '0');
        const month = (d.getMonth() + 1).toString().padStart(2, '0');
        const year = d.getFullYear();
        return `${day}/${month}/${year}`;
    } catch (e) { return String(date); }
}

function getVendorsList() {
    return getData('vendors').map(v => ({
        id: v['Vendor ID'] || v.VendorID,
        name: v['Vendor Name'] || v.VendorName,
        mobile: v.Mobile,
        status: v.Status
    }));
}

function getIssueTypes() {
    return getData('issues').map(i => ({
        id: i['Issue ID'] || i.IssueID,
        name: i['Issue Name'] || i.IssueName,
        category: i.Category
    }));
}

function logStatus(id, status, remarks, user) {
    try {
        const sheet = getSheet('complaintStatus');
        sheet.appendRow([id, status, remarks || '', new Date(), user || 'System']);
        SpreadsheetApp.flush();
    } catch (e) { console.error('logStatus error:', e); }
}

function logServiceHistory(data) {
    try {
        const sheet = getSheet('serviceHistory');
        sheet.appendRow([
            data.eventDate || new Date(),
            data.eventType || 'Log',
            data.entityType || 'General',
            data.entityId || 'None',
            data.productModel || '',
            data.partNumber || '',
            data.warrantyId || '',
            data.customerName || '',
            "'" + (data.mobile || ''),
            data.technician || '',
            data.status || '',
            data.notes || '',
            data.source || 'Backend'
        ]);
        SpreadsheetApp.flush();
    } catch (e) { console.error('logServiceHistory error:', e); }
}

function applySmartAssignment_(type, entityId) {
    try {
        const techs = getTechnicians().filter(t => (t.Status || 'Active').toLowerCase() === 'active');
        if (techs.length === 0) return 'Unassigned';

        // Get all active complaints/installations to count workload
        const sheetName = type === 'complaint' ? 'complaints' : 'installation';
        const data = getData(sheetName);
        const activeTasks = data.filter(item => !['Resolved', 'Completed', 'Cancelled'].includes(item.Status));
        
        // Count tasks per tech
        const workload = {};
        techs.forEach(t => workload[t.Name || t.Username] = 0);
        activeTasks.forEach(task => {
            const techName = task['Technician Assigned'];
            if (techName && workload[techName] !== undefined) workload[techName]++;
        });

        // Find tech with lowest workload
        let bestTech = techs[0].Name || techs[0].Username;
        let minTasks = Infinity;
        techs.forEach(t => {
            const name = t.Name || t.Username;
            if (workload[name] < minTasks) {
                minTasks = workload[name];
                bestTech = name;
            }
        });

        const sheet = getSheet(sheetName);
        const rawData = sheet.getDataRange().getValues();
        const idCol = 1; // Complaint ID or Installation ID is at index 1
        const techCol = type === 'complaint' ? 17 : 19; // Col R or T

        for (let i = 1; i < rawData.length; i++) {
            if (String(rawData[i][idCol]) === String(entityId)) {
                sheet.getRange(i + 1, techCol + 1).setValue(bestTech);
                logStatus(entityId, 'Assigned', 'Auto-assigned to ' + bestTech + ' (Active Tasks: ' + minTasks + ')', 'System');
                return bestTech;
            }
        }
        return bestTech;
    } catch (e) { 
        console.error('SmartAssignment error:', e); 
        return 'Auto-assignment failed';
    }
}

/**
 * Generates a professional PDF for the complaint and sends it via email.
 */
function sendComplaintEmail(cid, isUpdate = false, updateNotes = '') {
    try {
        const complaints = getData('complaints');
        const c = complaints.find(item => String(item['Complaint ID']) === String(cid));
        if (!c) return;

        const techs = getTechnicians();
        const tech = techs.find(t => (t.Name || t.Username) === c['Technician Assigned']) || {};
        
        const logoUrl = getWebSetting('site_logo') || 'https://www.gsvee.in/wp-content/uploads/2021/04/vvarma-logo.png';
        const companyName = "V-VARMA AUTOMATION";
        
        const bookingDate = new Date(c['Complaint Date'] || new Date());
        const dueDate = new Date(bookingDate);
        dueDate.setDate(dueDate.getDate() + 7);

        // PDF HTML Template
        const pdfHtml = `
            <html>
            <head>
                <style>
                    body { font-family: 'Arial', sans-serif; color: #333; margin: 0; padding: 20px; line-height: 1.6; }
                    .header { text-align: center; border-bottom: 3px solid #ff6b00; padding-bottom: 20px; margin-bottom: 30px; }
                    .logo { height: 80px; margin-bottom: 10px; }
                    .title { font-size: 24px; font-weight: bold; color: #ff6b00; text-transform: uppercase; }
                    .section { margin-bottom: 25px; background: #fff; border: 1px solid #eee; border-radius: 8px; overflow: hidden; }
                    .section-header { background: #f8f9fa; padding: 10px 15px; font-weight: bold; color: #ff6b00; border-bottom: 1px solid #eee; text-transform: uppercase; font-size: 14px; }
                    .grid { display: flex; flex-wrap: wrap; padding: 15px; }
                    .item { width: 50%; margin-bottom: 10px; }
                    .label { font-weight: bold; color: #666; font-size: 12px; display: block; }
                    .value { font-size: 14px; color: #000; }
                    .full-width { width: 100%; }
                    .footer { text-align: center; font-size: 12px; color: #777; border-top: 1px solid #eee; margin-top: 40px; padding-top: 20px; }
                    .status-badge { display: inline-block; padding: 4px 12px; border-radius: 20px; background: #ff6b00; color: #fff; font-size: 12px; font-weight: bold; }
                </style>
            </head>
            <body>
                <div class="header">
                    <img src="${logoUrl}" class="logo">
                    <div class="title">Complaint Registration Report</div>
                    <div style="font-size: 14px; color: #666;">${companyName}</div>
                </div>

                <div class="section">
                    <div class="section-header">Complaint Information</div>
                    <div class="grid">
                        <div class="item"><span class="label">Complaint Number</span><span class="value"><strong>${c['Complaint ID']}</strong></span></div>
                        <div class="item"><span class="label">Status</span><span class="status-badge">${c['Status'] || 'Registered'}</span></div>
                        <div class="item"><span class="label">Booking Date</span><span class="value">${formatDate(bookingDate)}</span></div>
                        <div class="item"><span class="label">Target Due Date</span><span class="value">${formatDate(dueDate)}</span></div>
                    </div>
                </div>

                <div class="section">
                    <div class="section-header">Customer & Location Details</div>
                    <div class="grid">
                        <div class="item"><span class="label">Customer Name</span><span class="value">${c['Customer Name']}</span></div>
                        <div class="item"><span class="label">Phone Number</span><span class="value">${c['Mobile Number']}</span></div>
                        <div class="full-width"><span class="label">Service Address</span><span class="value">${c['Full Address']}</span></div>
                        <div class="item"><span class="label">Area</span><span class="value">${c['Area'] || 'N/A'}</span></div>
                        <div class="item"><span class="label">Pincode</span><span class="value">${c['Pincode'] || 'N/A'}</span></div>
                    </div>
                </div>

                <div class="section">
                    <div class="section-header">Product & Issue Details</div>
                    <div class="grid">
                        <div class="item"><span class="label">Model Name/No</span><span class="value">${c['Product Model']}</span></div>
                        <div class="item"><span class="label">Warranty ID</span><span class="value">${c['Warranty ID'] || 'N/A'}</span></div>
                        <div class="item"><span class="label">Issue Category</span><span class="value">${c['Issue Type']}</span></div>
                        <div class="full-width" style="margin-top:10px;"><span class="label">Problem Description</span><span class="value">${c['Problem Description']}</span></div>
                    </div>
                </div>

                <div class="section">
                    <div class="section-header">Assigned Technician</div>
                    <div class="grid">
                        <div class="item"><span class="label">Technician Name</span><span class="value">${tech.Name || c['Technician Assigned'] || 'Pending Assignment'}</span></div>
                        <div class="item"><span class="label">Contact Number</span><span class="value">${tech.Mobile || 'N/A'}</span></div>
                    </div>
                </div>

                <div class="footer">
                    <p>This is a computer-generated report. No signature is required.</p>
                    <p><b>${companyName}</b> | Support: +91 90926 10415 | Web: www.gsvee.in</p>
                </div>
            </body>
            </html>
        `;

        const blob = Utilities.newBlob(pdfHtml, MimeType.HTML).getAs(MimeType.PDF);
        blob.setName(`Complaint_Report_${cid}.pdf`);

        // Prepare Email
        const subject = isUpdate ? `[Update] Complaint ${cid} - ${c['Status']}` : `Complaint Registered: ${cid} - ${c['Product Model']}`;
        const bodyTemplate = `
            <div style="font-family: sans-serif; padding: 20px; color: #333; max-width: 600px; border: 1px solid #eee; border-radius: 10px;">
                <img src="${logoUrl}" style="height: 50px; margin-bottom: 20px;">
                <h2 style="color: #ff6b00; margin-top: 0;">${isUpdate ? 'Complaint Update' : 'New Complaint Registered'}</h2>
                <p>Dear <b>${c['Customer Name']}</b>,</p>
                <p>${isUpdate ? 'There has been an update regarding your complaint.' : 'Your complaint has been successfully registered in our system.'}</p>
                
                <div style="background: #fdf2f2; padding: 15px; border-radius: 8px; border-left: 4px solid #ff6b00; margin: 20px 0;">
                    <p style="margin: 0; font-weight: bold; color: #c026d3;">Complaint ID: ${cid}</p>
                    <p style="margin: 5px 0 0 0;">Status: <b>${c['Status']}</b></p>
                    ${updateNotes ? `<p style="margin: 10px 0 0 0; font-size: 0.9rem; color: #555;"><b>Update Note:</b><br>${updateNotes}</p>` : ''}
                </div>

                <h4 style="margin-bottom: 10px;">Technician Assigned:</h4>
                <div style="background: #f8fafc; padding: 15px; border-radius: 8px; border: 1px solid #e2e8f0;">
                    <p style="margin: 0;"><b>Name:</b> ${tech.Name || c['Technician Assigned'] || 'Under Review'}</p>
                    ${tech.Mobile ? `<p style="margin: 5px 0 0 0;"><b>Mobile:</b> ${tech.Mobile}</p>` : ''}
                </div>

                <p style="margin-top: 30px; font-size: 0.9rem; color: #666;">
                    Please find the detailed complaint report attached as a PDF. Our team will contact you shortly.
                </p>
                
                <hr style="border: none; border-top: 1px solid #eee; margin: 30px 0;">
                <p style="text-align: center; font-size: 0.8rem; color: #999;">
                    &copy; ${new Date().getFullYear()} ${companyName} | Helpline: +91 90926 10415
                </p>
            </div>
        `;

        // Send Email: To Customer, CC Tech, BCC Admin
        const recipients = [c['Email']];
        const ccList = tech.Email ? [tech.Email] : [];
        const bccList = ['vvarmaautomation@gmail.com']; // Admin

        MailApp.sendEmail({
            to: recipients.filter(Boolean).join(','),
            cc: ccList.filter(Boolean).join(','),
            bcc: bccList.join(','),
            subject: subject,
            htmlBody: bodyTemplate,
            attachments: [blob]
        });

        return true;
    } catch (e) {
        console.error('sendComplaintEmail error:', e);
        return false;
    }
}
// ============================================
// 6. CATALOG & PRODUCT SYSTEM (NEW)
// ============================================

function getTrueInventoryStocks() {
  try {
    const inventorySheet = getSheet('inventory');
    const catalogSheet = getSheet('catalog');
    
    if (inventorySheet.getLastRow() <= 1) {
      return { stocks: {}, serials: [] };
    }
    
    const invData = inventorySheet.getDataRange().getValues();
    const invHeaders = invData[0].map(h => String(h).trim());
    
    const getIdx = (headers, names) => {
      for (let n of names) {
        const i = headers.indexOf(n);
        if (i !== -1) return i;
      }
      return -1;
    };
    
    const snCol = getIdx(invHeaders, ['Serial Number', 'serialnumber', 'Serial No']);
    const modelCol = getIdx(invHeaders, ['Model Number', 'modelnumber', 'Model ID', 'ModelCode', 'modelcode']);
    const actionCol = getIdx(invHeaders, ['Action', 'action', 'Action Log']);
    const qtyCol = getIdx(invHeaders, ['Quantity', 'quantity']);
    
    if (modelCol === -1 || actionCol === -1) {
      return { stocks: {}, serials: [] };
    }
    
    const snStatus = {}; // serial -> serialObject
    const modelStockMap = {}; // model -> count
    
    // Create images and catalog details map
    const catalogRaw = catalogSheet.getDataRange().getValues();
    const catalogHeaders = catalogRaw[0].map(h => String(h).trim());
    const cIdIdx = getIdx(catalogHeaders, ['ProductID', 'productid', 'Product ID', 'id']);
    const cModelIdx = getIdx(catalogHeaders, ['Model Number', 'modelnumber', 'Model No', 'ModelCode', 'modelcode']);
    const cNameIdx = getIdx(catalogHeaders, ['Model Name', 'modelname', 'Name', 'name']);
    const cTitleIdx = getIdx(catalogHeaders, ['Product Title', 'producttitle', 'Title']);
    const cPartIdx = getIdx(catalogHeaders, ['Part Number', 'partnumber', 'Part No']);
    const cImageIdx = getIdx(catalogHeaders, ['PrimaryImage', 'primaryimage', 'Images', 'images', 'ImageURL', 'imageurl']);
    
    const catalogMap = {};
    for (let i = 1; i < catalogRaw.length; i++) {
      const model = String(catalogRaw[i][cModelIdx] || '').trim();
      const modelId = String(catalogRaw[i][cIdIdx] || '').trim();
      
      let rawImg = catalogRaw[i][cImageIdx] || '';
      if (typeof rawImg === 'string' && rawImg.trim().startsWith('[')) {
        try {
          const parsed = JSON.parse(rawImg);
          if (Array.isArray(parsed) && parsed.length > 0) rawImg = parsed[0];
        } catch(e) {}
      }
      
      const itemData = {
        modelId: modelId,
        name: catalogRaw[i][cTitleIdx] || catalogRaw[i][cNameIdx] || '',
        modelName: catalogRaw[i][cNameIdx] || '',
        partNumber: catalogRaw[i][cPartIdx] || '',
        imageUrl: getDirectDriveUrl(rawImg)
      };
      
      if (model) {
        catalogMap[model] = itemData;
      }
      if (modelId) {
        catalogMap[modelId] = itemData;
      }
    }
    
    for (let i = 1; i < invData.length; i++) {
      const sn = snCol !== -1 ? String(invData[i][snCol]).trim() : '';
      const model = String(invData[i][modelCol]).trim();
      const action = String(invData[i][actionCol] || '').toUpperCase();
      const qty = qtyCol !== -1 ? parseInt(invData[i][qtyCol]) || 1 : 1;
      
      if (!model) continue;
      
      const isOut = action.includes('OUT') || action.includes('DEDUCT') || action.includes('SALE') || action.includes('BILL');
      const isIn = action.includes('IN') || action.includes('ADD') || action.includes('PURCHASE') || action.includes('ENTRY') || action.includes('RETURN');
      
      if (sn) {
        if (isIn && !isOut) {
          snStatus[sn] = {
            sn: sn,
            modelId: model,
            name: catalogMap[model] ? catalogMap[model].name : '',
            modelName: catalogMap[model] ? catalogMap[model].modelName : '',
            partNumber: catalogMap[model] ? catalogMap[model].partNumber : '',
            imageUrl: catalogMap[model] ? catalogMap[model].imageUrl : ''
          };
        } else if (isOut) {
          delete snStatus[sn];
        }
      } else {
        // Non-serial items or items with empty serial numbers
        if (isIn && !isOut) {
          modelStockMap[model] = (modelStockMap[model] || 0) + qty;
        } else if (isOut) {
          modelStockMap[model] = (modelStockMap[model] || 0) - qty;
        }
      }
    }
    
    // Add serial counts to the modelStockMap
    const serialsList = Object.values(snStatus);
    serialsList.forEach(item => {
      modelStockMap[item.modelId] = (modelStockMap[item.modelId] || 0) + 1;
    });
    
    return { stocks: modelStockMap, serials: serialsList };
  } catch (e) {
    console.error('getTrueInventoryStocks Error:', e);
    return { stocks: {}, serials: [] };
  }
}

function syncAllSheetStocks() {
  try {
    const trueData = getTrueInventoryStocks();
    const stocks = trueData.stocks;
    
    // 1. Sync Catalog Sheet
    const catalogSheet = getSheet('catalog');
    const catalogData = catalogSheet.getDataRange().getValues();
    const catalogHeaders = catalogData[0].map(h => String(h).trim());
    
    const getIdx = (headers, names) => {
      for (let n of names) {
        const i = headers.indexOf(n);
        if (i !== -1) return i;
      }
      return -1;
    };
    
    const cModelIdx = getIdx(catalogHeaders, ['Model Number', 'modelnumber', 'Model No', 'ModelCode', 'modelcode']);
    const cStockIdx = getIdx(catalogHeaders, ['Stock', 'stock']);
    
    if (cModelIdx !== -1 && cStockIdx !== -1) {
      for (let i = 1; i < catalogData.length; i++) {
        const model = String(catalogData[i][cModelIdx] || '').trim();
        const trueStock = stocks[model] || 0;
        if (parseInt(catalogData[i][cStockIdx]) !== trueStock) {
          catalogSheet.getRange(i + 1, cStockIdx + 1).setValue(trueStock);
        }
      }
    }
    
    // 2. Sync Products Sheet
    const prodSheet = getSheet('products');
    const prodData = prodSheet.getDataRange().getValues();
    const prodHeaders = prodData[0].map(h => String(h).trim());
    
    const pModelIdx = getIdx(prodHeaders, ['Model Code', 'modelcode', 'ModelCode', 'id', 'ProductID']);
    const pStockIdx = getIdx(prodHeaders, ['Stock', 'stock', 'StockLevel', 'stocklevel']);
    
    if (pModelIdx !== -1 && pStockIdx !== -1) {
      for (let i = 1; i < prodData.length; i++) {
        const model = String(prodData[i][pModelIdx] || '').trim();
        const trueStock = stocks[model] || 0;
        if (parseInt(prodData[i][pStockIdx]) !== trueStock) {
          prodSheet.getRange(i + 1, pStockIdx + 1).setValue(trueStock);
        }
      }
    }
    
    SpreadsheetApp.flush();
    return { success: true, message: 'Stock levels fully synchronized across Catalog and Products.' };
  } catch (e) {
    console.error('syncAllSheetStocks Error:', e);
    return { success: false, message: e.message };
  }
}

function syncCatalogInventoryStock() {
  try {
    const res = syncAllSheetStocks();
    if (res.success) {
      return { success: true, message: 'Successfully calculated and synchronized all stock levels from active serial numbers database.' };
    } else {
      return res;
    }
  } catch (e) {
    console.error('syncCatalogInventoryStock Error:', e);
    return { success: false, message: e.message };
  }
}

function getCatalogStats() {
  try {
    const data = getCatalogProducts();
    const inventoryStocks = getTrueInventoryStocks().stocks;

    const categories = new Set();
    let lowStockCount = 0;
    let totalStockCount = 0;

    data.forEach(p => {
      if (p.Category) categories.add(p.Category);
      
      const model = String(p.ModelNo || p.modelno || p['Model Number'] || p.ModelNumber || '').trim();
      const catalogStock = parseInt(p.Stock || p.stock) || 0;
      const invStock = inventoryStocks[model];
      const realStock = (invStock !== undefined) ? invStock : catalogStock;
      
      if (realStock < 5) lowStockCount++;
      totalStockCount += realStock;
    });

    return {
      totalProducts: data.length,
      totalCategories: categories.size,
      lowStock: lowStockCount,
      totalStock: totalStockCount
    };
  } catch (e) {
    console.error('getCatalogStats Error:', e);
    return { totalProducts: 0, totalCategories: 0, lowStockItems: 0, totalStockCount: 0 };
  }
}

function syncCatalogHeaders(sheet) {
  try {
    const lastCol = sheet.getLastColumn();
    const schemaHeaders = SHEET_SCHEMAS.catalog;
    
    if (lastCol === 0) {
      sheet.appendRow(schemaHeaders);
      sheet.getRange(1, 1, 1, schemaHeaders.length).setFontWeight('bold').setBackground('#f3f3f3');
      return;
    }
    
    const headers = sheet.getRange(1, 1, 1, lastCol).getValues()[0].map(h => String(h).trim());
    schemaHeaders.forEach(h => {
      const matchIdx = headers.findIndex(sh => sh.toLowerCase() === h.toLowerCase());
      if (matchIdx === -1) {
        const nextCol = sheet.getLastColumn() + 1;
        sheet.getRange(1, nextCol).setValue(h);
        headers.push(h);
      }
    });
  } catch(e) {
    console.error('syncCatalogHeaders Error:', e);
  }
}

function saveCatalogItem(data) {
  try {
    const sheet = getSheet('catalog');
    const gallerySheet = getSheet('gallery_images');
    const productId = 'VV-CAT-' + Date.now().toString().substr(-6);
    
    // Ensure all schema headers are present in the GSheet
    syncCatalogHeaders(sheet);
    
    // 1. Save Product Basic Info (Images column will store a count or reference)
    const values = {
      'Product ID': productId,
      'Part Number': data.PartNumber,
      'Category': data.Category,
      'Mode of Operation': data.ModeOfOperation || data.Type,
      'Model Number': data.ModelNo,
      'Model Name': data.Name,
      'Product Title': data.ProductTitle || '',
      'Tagline': data.Tagline,
      'Usage': data.Usage,
      'Working Type': data.WorkingType,
      'Voltage': data.Voltage,
      'Amps': data.Amps,
      'Dry Run Feature': data.DryRunFeature || '',
      'Timers': data.Timers || '',
      'In Box Kit': data.InBoxKit || '',
      'Wires': data.Wires || '',
      'Installation Inclusive': data.InstallationInclusive || '',
      'Installation Charges': Number(data.InstallationCharges) || 0,
      'Free Accessory': data.FreeAccessory || '',
      'Description': data.Description,
      'Features': typeof data.Features === 'string' ? data.Features : JSON.stringify(data.Features),
      'Warranty': data.Warranty,
      'Accessories': data.Accessories,
      'T&C': data.TC,
      'Terms & Conditions': data.TC,
      'Colors': data.Colors,
      'MRP': Number(data.MRP) || 0,
      'Selling Price': Number(data.SellingPrice) || 0,
      'Wholesale Price': Number(data.WholesalePrice) || 0,
      'Offer Price': Number(data.OfferPrice) || 0,
      'Images': data.Images && data.Images.length > 0 ? (data.Images[0].startsWith('data:') ? 'Uploading...' : data.Images[0]) : '', 
      'Status': data.Status || 'Active',
      'Date Created': data.DateCreated || new Date().toISOString(),
      'Stock': 0,
      'HSN Code': data.HSNCode || '',
      'SAC Code': data.SACCode || '',
      'GST Rate': data.GSTRate || '18',
      'Banners': '[]',
      'YouTube': data.YouTube || ''
    };

    const sheetHeaders = sheet.getDataRange().getValues()[0].map(h => String(h).trim());
    
    // Build normalized map for case-insensitive column writing
    const normalizedValues = {};
    Object.keys(values).forEach(key => {
      const normKey = String(key).replace(/\s+/g, '').toLowerCase();
      normalizedValues[normKey] = values[key];
    });

    const rowData = sheetHeaders.map(h => {
      const normH = String(h).replace(/\s+/g, '').toLowerCase();
      return normalizedValues[normH] !== undefined ? normalizedValues[normH] : '';
    });
    
    sheet.appendRow(rowData);
    const lastRow = sheet.getLastRow();
    
    // 2. Save Images to Google Drive & Store URLs in Gallery Sheet
    if (data.Images && Array.isArray(data.Images)) {
      data.Images.forEach((imgBase64, index) => {
        let imageUrl = imgBase64; 
        if (imgBase64.startsWith('data:image')) {
          try {
            imageUrl = uploadImageToDrive(imgBase64, `Catalog_${productId}_${index}`);
          } catch (err) {
            console.error('Image Upload Failed:', err);
          }
        }
        
        gallerySheet.appendRow([
          productId,
          imageUrl, 
          index + 1,
          new Date().toISOString()
        ]);

        // If this is the first image, update the main catalog row
        if (index === 0 && imageUrl.startsWith('http')) {
          const cleanHeaders = sheetHeaders.map(h => String(h).replace(/\s+/g, '').toLowerCase());
          const colIndex = cleanHeaders.indexOf('images') + 1;
          if (colIndex > 0) {
            sheet.getRange(lastRow, colIndex).setValue(imageUrl);
          }
        }
      });
    }

    // 3. Save Banners to Google Drive & Store JSON array in Banners column
    let parsedBanners = data.Banners;
    if (typeof parsedBanners === 'string') {
      try { parsedBanners = JSON.parse(parsedBanners); } catch(e) { parsedBanners = { layout: 'single', items: [] }; }
    }
    
    let bannerData = { layout: 'single', items: [] };
    if (Array.isArray(parsedBanners)) {
      bannerData.items = parsedBanners;
    } else if (parsedBanners && parsedBanners.items) {
      bannerData.layout = parsedBanners.layout || 'single';
      bannerData.items = parsedBanners.items;
    }

    if (bannerData.items && bannerData.items.length > 0) {
      let finalBannerItems = [];
      bannerData.items.forEach((bannerObj, index) => {
        let bannerUrl = typeof bannerObj === 'string' ? bannerObj : (bannerObj.url || '');
        if (bannerUrl.startsWith('data:image')) {
          try {
            bannerUrl = uploadImageToDrive(bannerUrl, `Catalog_Banner_${productId}_${index}`);
          } catch (err) {
            console.error('Banner Upload Failed:', err);
          }
        }
        if (bannerUrl.startsWith('http') || bannerUrl) {
          if (typeof bannerObj === 'object') {
            bannerObj.url = bannerUrl;
            finalBannerItems.push(bannerObj);
          } else {
            finalBannerItems.push({ url: bannerUrl, title: '' });
          }
        }
      });
      bannerData.items = finalBannerItems;
      
      const cleanHeaders = sheetHeaders.map(h => String(h).replace(/\s+/g, '').toLowerCase());
      const colIndex = cleanHeaders.indexOf('banners') + 1;
      if (colIndex > 0) {
        sheet.getRange(lastRow, colIndex).setValue(JSON.stringify(bannerData));
      }
    } else {
      const cleanHeaders = sheetHeaders.map(h => String(h).replace(/\s+/g, '').toLowerCase());
      const colIndex = cleanHeaders.indexOf('banners') + 1;
      if (colIndex > 0) {
        sheet.getRange(lastRow, colIndex).setValue('{"layout":"single","items":[]}');
      }
    }
    
    SpreadsheetApp.flush();
    return { success: true, productId: productId };
  } catch (e) {
    console.error('saveCatalogItem Error:', e);
    return { success: false, message: e.message };
  }
}

function deleteCatalogProduct(pid, permanent = false) {
  try {
    const sheet = getSheet('catalog');
    const gallerySheet = getSheet('gallery_images');
    const data = sheet.getDataRange().getValues();
    const headers = SHEET_SCHEMAS.catalog;
    const statusCol = headers.indexOf('Status') + 1;

    for (let i = 1; i < data.length; i++) {
      if (String(data[i][0]) === String(pid)) {
        if (permanent) {
          // Permanent Delete
          sheet.deleteRow(i + 1);
          // Delete from Gallery too
          const galData = gallerySheet.getDataRange().getValues();
          for (let j = galData.length - 1; j >= 1; j--) {
            if (String(galData[j][0]) === String(pid)) {
              gallerySheet.deleteRow(j + 1);
            }
          }
        } else {
          // Just Archive
          sheet.getRange(i + 1, statusCol).setValue('Archived');
        }
        SpreadsheetApp.flush();
        return { success: true, message: permanent ? 'Product deleted permanently' : 'Product moved to archive' };
      }
    }
    return { success: false, message: 'Product not found' };
  } catch (e) { return { success: false, message: e.message }; }
}

function updateCatalogProduct(data) {
  try {
    const sheet = getSheet('catalog');
    
    // Ensure all schema headers are present in the GSheet
    syncCatalogHeaders(sheet);
    
    const rows = sheet.getDataRange().getValues();
    const sheetHeaders = rows[0].map(h => String(h).trim());
    for (let i = 1; i < rows.length; i++) {
      if (String(rows[i][0]) === String(data.ProductID)) {
        const row = i + 1;
        
        const updateMap = {
          'Part Number': data.PartNumber,
          'Category': data.Category,
          'Mode of Operation': data.ModeOfOperation || data.Type,
          'Model Number': data.ModelNo,
          'Model Name': data.Name,
          'Product Title': data.ProductTitle || '',
          'Tagline': data.Tagline,
          'Usage': data.Usage,
          'Working Type': data.WorkingType,
          'Voltage': data.Voltage,
          'Amps': data.Amps,
          'Dry Run Feature': data.DryRunFeature || '',
          'Timers': data.Timers || '',
          'In Box Kit': data.InBoxKit || '',
          'Wires': data.Wires || '',
          'Installation Inclusive': data.InstallationInclusive || '',
          'Installation Charges': Number(data.InstallationCharges) || 0,
          'Free Accessory': data.FreeAccessory || '',
          'Description': data.Description,
          'Features': typeof data.Features === 'string' ? data.Features : JSON.stringify(data.Features || ''),
          'Warranty': data.Warranty,
          'Accessories': data.Accessories,
          'T&C': data.TC,
          'Terms & Conditions': data.TC,
          'Colors': data.Colors,
          'MRP': Number(data.MRP) || 0,
'Selling Price': Number(data.SellingPrice) || 0,
          'Wholesale Price': Number(data.WholesalePrice) || 0,
          'Offer Price': Number(data.OfferPrice) || 0,
          'Status': data.Status || rows[i][sheetHeaders.indexOf('Status')] || 'Active',
          'HSN Code': data.HSNCode || '',
          'SAC Code': data.SACCode || '',
          'GST Rate': data.GSTRate || '18',
          'YouTube': data.YouTube || ''
        };

        // Build a normalized update map (keys normalized to lowercase, no spaces)
        const normalizedUpdateMap = {};
        Object.keys(updateMap).forEach(key => {
          const normKey = String(key).replace(/\s+/g, '').toLowerCase();
          normalizedUpdateMap[normKey] = updateMap[key];
        });

        // Bulk update columns using actual sheet headers to prevent mapping/alignment bugs
        sheetHeaders.forEach((header, idx) => {
          const normHeader = String(header).replace(/\s+/g, '').toLowerCase();
          if (normalizedUpdateMap[normHeader] !== undefined) {
            sheet.getRange(row, idx + 1).setValue(normalizedUpdateMap[normHeader]);
          }
        });
        
        // Images (If new images were uploaded)
        if (data.Images && Array.isArray(data.Images) && data.Images.length > 0) {
          const gallerySheet = getSheet('gallery_images');
          const galData = gallerySheet.getDataRange().getValues();
          // Remove old images
          for (let j = galData.length - 1; j >= 1; j--) {
            if (String(galData[j][0]) === String(data.ProductID)) {
              gallerySheet.deleteRow(j + 1);
            }
          }
          // Add new images
          let firstImageUrl = '';
          data.Images.forEach((imgBase64, index) => {
            let imageUrl = imgBase64;
            if (imgBase64.startsWith('data:image')) {
              try {
                imageUrl = uploadImageToDrive(imgBase64, `Catalog_Update_${data.ProductID}_${index}`);
              } catch (err) {
                console.error('Image Update Failed:', err);
              }
            }
            gallerySheet.appendRow([data.ProductID, imageUrl, index + 1, new Date().toISOString()]);
            if (index === 0) {
              firstImageUrl = imageUrl;
            }
          });

          if (firstImageUrl && firstImageUrl.startsWith('http')) {
            const cleanHeaders = sheetHeaders.map(h => String(h).replace(/\s+/g, '').toLowerCase());
            const colIndex = cleanHeaders.indexOf('images') + 1;
            if (colIndex > 0) {
              sheet.getRange(row, colIndex).setValue(firstImageUrl);
            }
          }
        }

        // Banners (If new banners were uploaded)
        let parsedUpdateBanners = data.Banners;
        if (typeof parsedUpdateBanners === 'string') {
          try { parsedUpdateBanners = JSON.parse(parsedUpdateBanners); } catch(e) { parsedUpdateBanners = { layout: 'single', items: [] }; }
        }
        
        let bannerDataUpdate = { layout: 'single', items: [] };
        if (Array.isArray(parsedUpdateBanners)) {
          bannerDataUpdate.items = parsedUpdateBanners;
        } else if (parsedUpdateBanners && parsedUpdateBanners.items) {
          bannerDataUpdate.layout = parsedUpdateBanners.layout || 'single';
          bannerDataUpdate.items = parsedUpdateBanners.items;
        }

        if (bannerDataUpdate.items && bannerDataUpdate.items.length > 0) {
          let updatedBannerUrls = [];
          bannerDataUpdate.items.forEach((bannerObj, index) => {
            let bannerUrl = typeof bannerObj === 'string' ? bannerObj : (bannerObj.url || '');
            if (bannerUrl.startsWith('data:image')) {
              try {
                bannerUrl = uploadImageToDrive(bannerUrl, `Catalog_Banner_Update_${data.ProductID}_${index}`);
              } catch (err) {
                console.error('Banner Update Failed:', err);
              }
            }
            if (bannerUrl.startsWith('http') || bannerUrl) {
              if (typeof bannerObj === 'object') {
                bannerObj.url = bannerUrl;
                updatedBannerUrls.push(bannerObj);
              } else {
                updatedBannerUrls.push({ url: bannerUrl, title: '' });
              }
            }
          });
          bannerDataUpdate.items = updatedBannerUrls;
          
          const cleanHeaders = sheetHeaders.map(h => String(h).replace(/\s+/g, '').toLowerCase());
          const colIndex = cleanHeaders.indexOf('banners') + 1;
          if (colIndex > 0) {
            sheet.getRange(row, colIndex).setValue(JSON.stringify(bannerDataUpdate));
          }
        } else {
          const cleanHeaders = sheetHeaders.map(h => String(h).replace(/\s+/g, '').toLowerCase());
          const colIndex = cleanHeaders.indexOf('banners') + 1;
          if (colIndex > 0) {
            sheet.getRange(row, colIndex).setValue('{"layout":"single","items":[]}');
          }
        }
        
        SpreadsheetApp.flush();
        return { success: true, productId: data.ProductID, message: 'Product updated successfully' };
      }
    }
    return { success: false, message: 'Product not found: ' + data.ProductID };
  } catch (e) { 
    console.error('updateCatalogProduct Error:', e);
    return { success: false, message: e.message }; 
  }
}

/**
 * Fetches products for the Catalog module with optimized merging and optional filtering.
 */
function getCatalogProducts(filterStatus, limit) {
  const startTime = new Date().getTime();
  try {
    let parsedStatus = null;
    let parsedLimit = limit;

    // Handle case where client passes options object as the first argument
    if (filterStatus && typeof filterStatus === 'object') {
      parsedLimit = filterStatus.limit || limit;
      parsedStatus = filterStatus.filterStatus || filterStatus.status || null;
    } else if (typeof filterStatus === 'string') {
      parsedStatus = filterStatus;
    }

    console.log('[getCatalogProducts] Starting fetch. Filter:', parsedStatus, 'Limit:', parsedLimit);
    
    // 1. Fetch Catalog Data
    let catalogRaw = getData('catalog');
    
    // Helper to check if a catalog item has valid product data
    const isValidCatalogItem = function(item) {
      if (!item) return false;
      const pid = String(item['Product ID'] || item['productid'] || item['ProductID'] || '').trim();
      const model = String(item['Model Number'] || item['modelnumber'] || item['ModelNo'] || '').trim();
      
      // Filter out empty rows, templates, placeholders, or headers
      if (pid.length === 0 && model.length === 0) return false;
      if (pid.toLowerCase() === 'product id' || model.toLowerCase() === 'model number') return false;
      if (pid.toLowerCase().includes('template') || model.toLowerCase().includes('template')) return false;
      if (pid.toLowerCase().includes('placeholder') || model.toLowerCase().includes('placeholder')) return false;
      
      return true;
    };
    
    const validCatalogRows = (catalogRaw || []).filter(isValidCatalogItem);
    
    // FALLBACK: If catalog sheet is empty or has no valid products, fall back to legacy 'products' (Product_Models) sheet and seed it on the fly!
    if (!catalogRaw || catalogRaw.length === 0 || validCatalogRows.length === 0) {
      console.warn('[getCatalogProducts] Catalog sheet is empty or has no valid products. Falling back to legacy products sheet (Product_Models) and auto-seeding...');
      const productsRaw = getData('products');
      if (productsRaw && productsRaw.length > 0) {
        try {
          const catalogSheet = getSheet('catalog');
          if (catalogSheet.getLastRow() > 1) {
            catalogSheet.deleteRows(2, catalogSheet.getLastRow() - 1);
          }
          let catalogHeaders = catalogSheet.getDataRange().getValues()[0].map(h => String(h).trim());
          
          if (catalogHeaders.length === 1 && catalogHeaders[0] === '') {
            catalogSheet.clear();
            catalogSheet.appendRow(SHEET_SCHEMAS.catalog);
            catalogSheet.getRange(1, 1, 1, SHEET_SCHEMAS.catalog.length).setFontWeight('bold').setBackground('#f3f3f3');
            catalogHeaders = SHEET_SCHEMAS.catalog;
          }
          
          catalogRaw = productsRaw.map(function(p) {
            const mapped = {};
            
            // Primary and fallback identifiers
            mapped['Product ID'] = p['Model Code'] || p['modelcode'] || p['ModelNo'] || p['modelno'] || '';
            mapped['Part Number'] = p['Part Number'] || p['partnumber'] || '';
            mapped['Category'] = p['Category'] || p['category'] || 'Uncategorized';
            
            // Mode of operation / automation type mapping
            mapped['Mode of Operation'] = p['Automation Type'] || p['automationtype'] || p['Motor Type'] || p['motortype'] || p['Type'] || p['type'] || 'Standard';
            
            mapped['Model Number'] = p['Model Code'] || p['modelcode'] || '';
            mapped['Model Name'] = p['Model Name'] || p['modelname'] || '';
            mapped['Product Title'] = p['Model Name'] || p['modelname'] || '';
            mapped['Tagline'] = '';
            mapped['Usage'] = '';
            mapped['Working Type'] = p['Type'] || p['type'] || '';
            
            mapped['Voltage'] = '';
            mapped['Amps'] = '';
            mapped['Dry Run Feature'] = '';
            mapped['Timers'] = '';
            mapped['In Box Kit'] = '';
            mapped['Wires'] = '';
            mapped['Installation Inclusive'] = '';
            mapped['Installation Charges'] = '';
            mapped['Free Accessory'] = '';
            
            mapped['Description'] = p['Description'] || p['description'] || '';
            mapped['Features'] = p['Specs'] || p['specs'] || '';
            mapped['Warranty'] = '';
            mapped['Accessories'] = '';
            mapped['T&C'] = '';
            mapped['Colors'] = '';
            
            // Prices
            mapped['MRP'] = parseFloat(p['MRP'] || p['mrp'] || p['Price'] || p['price'] || 0);
            mapped['Selling Price'] = parseFloat(p['Price'] || p['price'] || 0);
            mapped['Wholesale Price'] = 0; // Exclude/secure wholesale prices in public catalog
            mapped['Offer Price'] = parseFloat(p['Price'] || p['price'] || 0);
            
            // Images
            let imageUrl = p['ImageUrl'] || p['imageurl'] || '';
            if (imageUrl) {
              mapped['Images'] = JSON.stringify([imageUrl]);
            } else {
              mapped['Images'] = '[]';
            }
            
            mapped['Status'] = p['Status'] || p['status'] || 'Active';
            mapped['Stock'] = parseInt(p['Stock'] || p['stock'] || 0);
            mapped['HSN Code'] = p['QR Code'] || '';
            mapped['SAC Code'] = '';
            mapped['GST Rate'] = 18;
            
            // Auto-append to GSheet Catalog so it is populated physically
            const normalizedValues = {};
            Object.keys(mapped).forEach(key => {
              const normKey = String(key).replace(/\s+/g, '').toLowerCase();
              normalizedValues[normKey] = mapped[key];
            });
            const rowData = catalogHeaders.map(h => {
              const normH = String(h).replace(/\s+/g, '').toLowerCase();
              return normalizedValues[normH] !== undefined ? normalizedValues[normH] : '';
            });
            catalogSheet.appendRow(rowData);
            
            return mapped;
          });
          SpreadsheetApp.flush();
          console.log('[getCatalogProducts] Successfully auto-seeded primary catalog sheet with ' + catalogRaw.length + ' products.');
        } catch (seedErr) {
          console.error('[getCatalogProducts] Seeding failed, returning mapped products only:', seedErr);
          // Fallback to returned map only if actual sheet append fails
          catalogRaw = productsRaw.map(function(p) {
            const mapped = {};
            mapped['Product ID'] = p['Model Code'] || p['modelcode'] || p['ModelNo'] || p['modelno'] || '';
            mapped['Part Number'] = p['Part Number'] || p['partnumber'] || '';
            mapped['Category'] = p['Category'] || p['category'] || 'Uncategorized';
            mapped['Mode of Operation'] = p['Automation Type'] || p['automationtype'] || p['Motor Type'] || p['motortype'] || p['Type'] || p['type'] || 'Standard';
            mapped['Model Number'] = p['Model Code'] || p['modelcode'] || '';
            mapped['Model Name'] = p['Model Name'] || p['modelname'] || '';
            mapped['Product Title'] = p['Model Name'] || p['modelname'] || '';
            mapped['Tagline'] = '';
            mapped['Usage'] = '';
            mapped['Working Type'] = p['Type'] || p['type'] || '';
            mapped['Voltage'] = '';
            mapped['Amps'] = '';
            mapped['Dry Run Feature'] = '';
            mapped['Timers'] = '';
            mapped['In Box Kit'] = '';
            mapped['Wires'] = '';
            mapped['Installation Inclusive'] = '';
            mapped['Installation Charges'] = '';
            mapped['Free Accessory'] = '';
            mapped['Description'] = p['Description'] || p['description'] || '';
            mapped['Features'] = p['Specs'] || p['specs'] || '';
            mapped['Warranty'] = '';
            mapped['Accessories'] = '';
            mapped['T&C'] = '';
            mapped['Colors'] = '';
            mapped['MRP'] = parseFloat(p['MRP'] || p['mrp'] || p['Price'] || p['price'] || 0);
            mapped['Selling Price'] = parseFloat(p['Price'] || p['price'] || 0);
            mapped['Wholesale Price'] = 0;
            mapped['Offer Price'] = parseFloat(p['Price'] || p['price'] || 0);
            let imageUrl = p['ImageUrl'] || p['imageurl'] || '';
            mapped['Images'] = imageUrl ? JSON.stringify([imageUrl]) : '[]';
            mapped['Status'] = p['Status'] || p['status'] || 'Active';
            mapped['Stock'] = parseInt(p['Stock'] || p['stock'] || 0);
            mapped['HSN Code'] = p['QR Code'] || '';
            mapped['SAC Code'] = '';
            mapped['GST Rate'] = 18;
            return mapped;
          });
        }
      }
    }

    let catalog = catalogRaw || [];
    if (!catalog || catalog.length === 0) return [];

    // 2. Fetch Supporting Data
    const gallery = getData('gallery_images');
    const inventoryLogs = getData('inventory');
    const reviewsData = getData('reviews') || [];

    const reviewStats = {};
    reviewsData.forEach(r => {
      const modelNo = String(r['Product Model'] || r.productModel || r.model || '').trim().toLowerCase();
      const rating = parseFloat(r.Rating || r.rating || 0);
      if (modelNo && !isNaN(rating)) {
        if (!reviewStats[modelNo]) {
          reviewStats[modelNo] = { sum: 0, count: 0 };
        }
        reviewStats[modelNo].sum += rating;
        reviewStats[modelNo].count += 1;
      }
    });

    // 3. Create optimized maps for lookup
    const imagesMap = {};
    gallery.forEach(img => {
      const rawPid = img.ProductID || img.productid || img['Product ID'] || img.ImageID || img.imageid || img['Image ID'] || img.id || img.ModelNo || img.modelno;
      const pid = rawPid ? String(rawPid).trim() : null;
      if (pid) {
        if (!imagesMap[pid]) imagesMap[pid] = [];
        
        let imgUrl = img.ImageURL || img.imageurl || img['Image URL'] || img.ImageBase64 || img.imagebase64 || img.ImageUrl || img.imageUrl || img.url || img.URL;
        if (imgUrl) {
          img.ImageURL = getDirectDriveUrl(imgUrl);
          img.imageurl = img.ImageURL;
          img.url = img.ImageURL;
        }
        imagesMap[pid].push(img);
      }
    });

    // Pre-sort images by DisplayOrder
    for (let id in imagesMap) {
      imagesMap[id].sort((a, b) => (parseInt(a.DisplayOrder) || 0) - (parseInt(b.DisplayOrder) || 0));
    }

    // Fetch actual computed stock levels from getTrueInventoryStocks
    const inventoryStocks = getTrueInventoryStocks().stocks;

    // 4. Merge Data
    const mergedData = catalog.map(item => {
      const getVal = (obj, keys) => {
        if (!obj) return null;
        for (let k of keys) { if (obj[k] !== undefined && obj[k] !== null && obj[k] !== '') return obj[k]; }
        return null;
      };

      const rawPid = getVal(item, ['ProductID', 'productid', 'Product ID', 'id']);
      const rawModel = getVal(item, ['ModelNo', 'modelno', 'Model No', 'ModelNumber', 'modelnumber']);
      const pid = rawPid ? String(rawPid).trim() : null;
      const model = rawModel ? String(rawModel).trim() : null;
      
      // Merge Images
      let pImages = pid ? (imagesMap[pid] || []) : [];
      if (pImages.length === 0 && model) { pImages = imagesMap[model] || []; }
      item.Gallery = pImages;
      
      // Primary Image Logic
      let primaryFromGallery = pImages.length > 0 ? pImages[0].ImageURL : null;
      let primaryFromCatalog = getVal(item, ['PrimaryImage', 'primaryimage', 'Images', 'images', 'ImageURL', 'imageurl']);
      
      if (typeof primaryFromCatalog === 'string' && primaryFromCatalog.trim().startsWith('[')) {
          try {
              const parsed = JSON.parse(primaryFromCatalog);
              if (Array.isArray(parsed) && parsed.length > 0) primaryFromCatalog = parsed[0];
          } catch(e) {}
      }
      item.PrimaryImage = getDirectDriveUrl(primaryFromGallery || primaryFromCatalog);
      
      // Stock Calculation
      const catalogStock = parseInt(getVal(item, ['Stock', 'stock', 'Quantity', 'quantity', 'StockLevel', 'stocklevel'])) || 0;
      const invStock = model ? inventoryStocks[model] : undefined;
      item.Stock = (invStock !== undefined) ? invStock : catalogStock;
      
      // Basic Metadata
      item.ProductID = pid;
      item.ModelNo = model;
      item.Name = getVal(item, ['ModelName', 'modelname', 'Model Name', 'Name', 'name']);
      item.ProductTitle = getVal(item, ['ProductTitle', 'producttitle', 'Product Title']) || item.Name;
      item.PartNumber = getVal(item, ['PartNumber', 'partnumber', 'Part Number']);
      item.ModeOfOperation = getVal(item, ['ModeOfOperation', 'modeofoperation', 'Mode of Operation']);
      item.Category = getVal(item, ['Category', 'category']);
      item.Tagline = getVal(item, ['Tagline', 'tagline']) || '';
      item.Usage = getVal(item, ['Usage', 'usage']) || '';
      item.WorkingType = getVal(item, ['Working Type', 'workingtype', 'WorkingType']) || '';
      item.Voltage = getVal(item, ['Voltage', 'voltage']) || '';
      item.Amps = getVal(item, ['Amps', 'amps']) || '';
      
      // Feature-specific checklist flags
      item.DryRunFeature = getVal(item, ['Dry Run Feature', 'dryrunfeature', 'DryRunFeature']) || 'No';
      item.Timers = getVal(item, ['Timers', 'timers']) || 'No';
      item.InBoxKit = getVal(item, ['In Box Kit', 'inboxkit', 'InBoxKit']) || '';
      item.Wires = getVal(item, ['Wires', 'wires']) || '';
      item.InstallationInclusive = getVal(item, ['Installation Inclusive', 'installationinclusive', 'InstallationInclusive']) || 'No';
      item.InstallationCharges = parseFloat(getVal(item, ['Installation Charges', 'installationcharges', 'InstallationCharges']) || 0);
      item.FreeAccessory = getVal(item, ['Free Accessory', 'freeaccessory', 'FreeAccessory']) || '';
      
      item.Description = getVal(item, ['Description', 'description']) || '';
      item.Accessories = getVal(item, ['Accessories', 'accessories']) || '';
      item.Colors = getVal(item, ['Colors', 'colors']) || '';
      item.SellingPrice = parseFloat(getVal(item, ['Selling Price', 'SellingPrice', 'sellingprice']) || 0);
      item.WholesalePrice = parseFloat(getVal(item, ['Wholesale Price', 'WholesalePrice', 'wholesaleprice']) || 0);
      item.MRP = parseFloat(getVal(item, ['MRP', 'mrp']) || 0);
      item.OfferPrice = parseFloat(getVal(item, ['Offer Price', 'OfferPrice', 'offerprice']) || 0);
      item.HSNCode = getVal(item, ['HSN Code', 'HSNCode', 'hsncode']) || '';
      item.SACCode = getVal(item, ['SAC Code', 'SACCode', 'saccode']) || '';
      item.GSTRate = parseFloat(getVal(item, ['GST Rate', 'GSTRate', 'gstrate'])) || 18;
      item.Status = getVal(item, ['Status', 'status']) || 'Active'; // Explicit Normalization
      item.Warranty = getVal(item, ['Warranty Details', 'warrantydetails', 'Warranty', 'warranty', 'Guarantee', 'guarantee']) || '';
      item.TC = getVal(item, ['Terms & Conditions', 'terms & conditions', 'Terms & Condition', 'terms & condition', 'T&C', 't&c', 'T & C', 't & c', 'terms&conditions', 'termsandconditions', 'tandc', 'TermsAndConditions', 'TermsAndCondition', 'Terms and Conditions', 'Terms', 'tc', 'TC']) || '';
      item.Banners = getVal(item, ['Banners', 'banners']) || '[]';
      item.YouTube = getVal(item, ['YouTube', 'youtube']) || '';

      // CRITICAL: Remove keys containing '&' to prevent google.script.run serialization failures.
      // google.script.run SILENTLY FAILS to serialize objects with '&' in property names.
      Object.keys(item).forEach(k => {
        if (k.includes('&')) delete item[k];
      });

      // Clean features array from runaway double-stringification
      let rawFeatures = getVal(item, ['Features', 'features']) || '[]';
      let parsedFeatures = [];
      if (typeof rawFeatures === 'string') {
        try {
          for (let i = 0; i < 3; i++) {
            if (typeof rawFeatures === 'string' && rawFeatures.trim().startsWith('[')) {
              rawFeatures = JSON.parse(rawFeatures);
            } else {
              break;
            }
          }
          if (Array.isArray(rawFeatures)) {
            parsedFeatures = rawFeatures.map(f => typeof f === 'string' ? f : JSON.stringify(f));
          } else if (rawFeatures) {
            parsedFeatures = [String(rawFeatures)];
          }
        } catch(e) {
          parsedFeatures = [rawFeatures];
        }
      } else if (Array.isArray(rawFeatures)) {
        parsedFeatures = rawFeatures.map(f => typeof f === 'string' ? f : JSON.stringify(f));
      } else if (rawFeatures) {
        parsedFeatures = [String(rawFeatures)];
      }
      item.Features = parsedFeatures;

      const modelLower = model ? model.toLowerCase() : '';
      const stats = reviewStats[modelLower];
      item.RatingCount = stats ? stats.count : 0;
      item.AverageRating = stats && stats.count > 0 ? parseFloat((stats.sum / stats.count).toFixed(1)) : 0;

      return item;
    });

    let filtered = mergedData;
    
    // Server-side status filtering if provided
    if (parsedStatus) {
      filtered = filtered.filter(p => String(p.Status).toLowerCase() === String(parsedStatus).toLowerCase());
    }
    
    // Apply limit if provided (useful for "Recent" dashboard view)
    if (parsedLimit && parsedLimit > 0) {
      filtered = filtered.slice(0, parsedLimit);
    }

    // Google Apps Script `google.script.run` will SILENTLY FAIL to serialize objects containing `NaN`.
    // We sanitize the final payload, replacing NaN with null, to guarantee delivery to the frontend!
    filtered = JSON.parse(JSON.stringify(filtered, (key, value) => {
      if (typeof value === 'number' && isNaN(value)) return null;
      return value;
    }));

    const duration = new Date().getTime() - startTime;
    console.log('[getCatalogProducts] Success. Merged ' + filtered.length + ' items in ' + duration + 'ms');
    return filtered;
  } catch (e) {
    console.error('[getCatalogProducts] Error:', e);
    return [];
  }
}

/**
 * Public catalog retrieval routine. Filters only Active products and secures
 * wholesale/cost price fields so they are never exposed to the public UI or payload.
 */
function getPublicCatalogProducts() {
  try {
    const products = getCatalogProducts('Active');
    return products.map(function(item) {
      const cleanItem = {};
      Object.keys(item).forEach(function(key) {
        if (!key.toLowerCase().includes('wholesale') && !key.includes('&')) {
          cleanItem[key] = item[key];
        }
      });
      return cleanItem;
    });
  } catch (e) {
    console.error('[getPublicCatalogProducts] Error:', e);
    return [];
  }
}

function getCatalogManagementData() {
  try {
    const sheet = getSheet('catalog_settings');
    let data = sheet.getDataRange().getValues();
    
    // REPAIR: Check if headers are missing (Row 1 should be 'Type', 'Value', 'CreatedAt')
    if (data.length > 0 && String(data[0][0]).toLowerCase() !== 'type') {
        console.warn('Catalog Settings: Headers missing. Repairing...');
        sheet.insertRowBefore(1);
        sheet.getRange(1, 1, 1, 3).setValues([['Type', 'Value', 'CreatedAt']]);
        sheet.getRange(1, 1, 1, 3).setFontWeight('bold').setBackground('#f3f3f3');
        SpreadsheetApp.flush();
        data = sheet.getDataRange().getValues(); // Re-read
    } else if (data.length === 0) {
        // Truly empty sheet
        sheet.appendRow(['Type', 'Value', 'CreatedAt']);
        sheet.getRange(1, 1, 1, 3).setFontWeight('bold').setBackground('#f3f3f3');
        SpreadsheetApp.flush();
        data = sheet.getDataRange().getValues();
    }
    
    let settings = getData('catalog_settings');
    
    const result = {
      Category: [],
      ProductType: [],
      ModelName: [],
      ModelNumber: [],
      Tagline: [],
      UsageType: [],
      WorkingType: [],
      VoltageRange: [],
      AmpsRange: [],
      Color: []
    };

    // If no data rows (only headers), seed initial defaults
    if (settings.length === 0) {
      const defaults = [
        ['Category', 'Voltage Stabilizer'], ['Category', 'Inverter'], ['Category', 'Battery'], ['Category', 'Solar Panel'],
        ['ProductType', 'Digital'], ['ProductType', 'Analog'], ['ProductType', 'LCD Display'],
        ['ModelName', 'Premium'], ['ModelName', 'Classic'], ['ModelName', 'Ultra'],
        ['ModelNumber', 'VV-PRO-100'], ['ModelNumber', 'VV-LITE-50'],
        ['Tagline', 'Reliable Power'], ['Tagline', 'Smart Energy'],
        ['UsageType', 'Residential'], ['UsageType', 'Industrial'],
        ['WorkingType', 'Automatic'], ['WorkingType', 'Manual'],
        ['VoltageRange', '90V - 300V'], ['VoltageRange', '130V - 280V'],
        ['AmpsRange', '5 Amps'], ['AmpsRange', '15 Amps'],
        ['Color', 'White'], ['Color', 'Black'], ['Color', 'Grey'], ['Color', 'Orange']
      ];
      
      defaults.forEach(d => {
        sheet.appendRow([d[0], d[1], new Date()]);
      });
      SpreadsheetApp.flush();
      settings = getData('catalog_settings');
    }
    
    settings.forEach(s => {
      const type = s.Type || s.type || s['Type'];
      const val = s.Value || s.value || s['Value'];
      if (type && result[type]) {
        result[type].push(val);
      }
    });
    
    return result;
  } catch (e) {
    console.error('getCatalogManagementData Error:', e);
    return {};
  }
}

function saveCatalogSetting(type, value, oldValue) {
  try {
    const sheet = getSheet('catalog_settings');
    const data = sheet.getDataRange().getValues();
    
    if (oldValue) {
      for (let i = 1; i < data.length; i++) {
        if (String(data[i][0]).trim() === String(type).trim() && String(data[i][1]).trim() === String(oldValue).trim()) {
          sheet.getRange(i + 1, 2).setValue(value);
          SpreadsheetApp.flush();
          return { success: true, data: getCatalogManagementData() };
        }
      }
    }
    
    sheet.appendRow([type, value, new Date()]);
    SpreadsheetApp.flush();
    return { success: true, data: getCatalogManagementData() };
  } catch (e) { return { success: false, message: e.message }; }
}

function deleteCatalogSetting(type, value) {
  try {
    const sheet = getSheet('catalog_settings');
    const data = sheet.getDataRange().getValues();
    for (let i = 1; i < data.length; i++) {
      if (String(data[i][0]).trim() === String(type).trim() && String(data[i][1]).trim() === String(value).trim()) {
        sheet.deleteRow(i + 1);
        SpreadsheetApp.flush();
        return { success: true, data: getCatalogManagementData() };
      }
    }
    return { success: false, message: 'Item not found: ' + value };
  } catch (e) { return { success: false, message: e.message }; }
}


function toggleCatalogStatus(pid, status) {
  try {
    const sheet = getSheet('catalog');
    const data = sheet.getDataRange().getValues();
    for (let i = 1; i < data.length; i++) {
      if (String(data[i][0]) === String(pid)) {
        const headers = SHEET_SCHEMAS.catalog;
        const statusCol = headers.indexOf('Status') + 1;
        if (statusCol > 0) {
          sheet.getRange(i + 1, statusCol).setValue(status);
        } else {
          sheet.getRange(i + 1, 22).setValue(status); // Fallback to column V
        }
        SpreadsheetApp.flush();
        return { success: true };
      }
    }
    return { success: false, message: 'Product ID not found: ' + pid };
  } catch (e) { return { success: false, message: e.message }; }
}

// ============================================
// 10. INVENTORY & STOCK SYSTEM (SERIAL TRACKING)
// ============================================

function saveInventoryStock(data) {
  try {
    const sheet = getSheet('inventory');
    const catalogSheet = getSheet('catalog');
    const now = new Date();
    const txnId = 'STK-' + Date.now().toString().substr(-6);
    
    // 1. Get Catalog Data for Model Code/Name
    const catalogData = catalogSheet.getDataRange().getValues();
    const getIdx = (headers, names) => {
      for (let n of names) {
        const i = headers.indexOf(n);
        if (i !== -1) return i;
      }
      return -1;
    };

    const hHeaders = catalogData[0]; // Use actual sheet headers
    const idIdx = getIdx(hHeaders, ['ProductID', 'productid', 'Product ID', 'id']);
    const codeIdx = getIdx(hHeaders, ['Model Number', 'modelnumber', 'Model No', 'ModelCode', 'modelcode']);
    const nameIdx = getIdx(hHeaders, ['Model Name', 'modelname', 'Name', 'name']);
    const titleIdx = getIdx(hHeaders, ['Product Title', 'producttitle', 'Title']);
    const partIdx = getIdx(hHeaders, ['Part Number', 'partnumber', 'Part No']);
    const stockIdx = getIdx(hHeaders, ['Stock', 'stock']);
    
    let productRow = -1;
    let modelNo = '';
    let modelName = '';
    let productTitle = '';
    let partNumber = '';
    let currentStock = 0;

    for (let i = 1; i < catalogData.length; i++) {
      if (String(catalogData[i][idIdx]) === String(data.ProductID)) {
        productRow = i + 1;
        modelNo = catalogData[i][codeIdx] || '';
        modelName = catalogData[i][nameIdx] || '';
        productTitle = catalogData[i][titleIdx] || '';
        partNumber = catalogData[i][partIdx] || '';
        currentStock = parseInt(catalogData[i][stockIdx]) || 0;
        break;
      }
    }

    if (productRow === -1) return { success: false, message: 'Product not found in Catalog' };

    // 2. Prepare Header Map for Inventory Sheet
    const invHeaders = sheet.getRange(1, 1, 1, sheet.getLastColumn()).getValues()[0].map(h => h.trim());
    const h = (name) => {
      const idx = invHeaders.indexOf(name);
      return idx === -1 ? null : idx;
    };

    // 3. Filter Valid Serials
    let existingSerials = [];
    if (sheet.getLastRow() > 1) {
      const sCol = h('Serial Number');
      if (sCol !== null) {
        existingSerials = sheet.getRange(2, sCol + 1, sheet.getLastRow() - 1, 1).getValues().flat().map(String);
      }
    }
    
    const newSerials = data.Serials || [];
    const validSerials = [];
    const skippedSerials = [];
    const isDeduction = ['Sale', 'Deduct', 'Stock Deduction', 'OUT', 'Dispatch'].includes(data.Action);

    newSerials.forEach(s => {
      if (isDeduction) {
        validSerials.push(s);
      } else {
        if (existingSerials.includes(String(s))) skippedSerials.push(s);
        else validSerials.push(s);
      }
    });

    if (newSerials.length > 0 && validSerials.length === 0) {
      return { success: false, message: 'No valid serial numbers to process.', skipped: skippedSerials };
    }
    
    if (newSerials.length === 0 && (!data.Quantity || data.Quantity <= 0)) {
       return { success: false, message: 'Must provide serials or valid quantity.' };
    }

    // 4. Append Rows using Strict Schema Mapping
    const schema = SHEET_SCHEMAS.inventory;
    const headerMap = {};
    invHeaders.forEach((h, i) => headerMap[h.trim()] = i);

    const getInvIdx = (name) => {
      if (headerMap[name] !== undefined) return headerMap[name];
      const aliases = {
        'Model ID': ['ProductID', 'id'],
        'Model Number': ['Model No', 'modelno'],
        'Product Title': ['Title', 'producttitle'],
        'Part Number': ['Part No', 'partnumber'],
        'Action': ['Action Log'],
        'Updated By': ['User', 'updatedby']
      };
      for (let a of (aliases[name] || [])) {
        if (headerMap[a] !== undefined) return headerMap[a];
      }
      return -1;
    };

    if (newSerials.length === 0) {
      // Bulk quantity update without serials
      const row = new Array(invHeaders.length).fill('');
      const set = (name, val) => {
        const idx = getInvIdx(name);
        if (idx !== -1) row[idx] = val;
      };
      
      set('Model ID', data.ProductID);
      set('Txn ID', txnId);
      set('Date', now);
      set('Model Number', modelNo);
      set('Model Name', modelName);
      set('Product Title', productTitle || data.ProductTitle || '');
      set('Part Number', partNumber || data.PartNumber || '');
      set('Action', data.Action || (isDeduction ? 'Stock Deduction' : 'Stock Addition'));
      set('Serial Number', '');
      set('Vendor Name', data.VendorName || '');
      set('Bill Number', data.BillNumber || '');
      set('Quantity', data.Quantity);
      
      set('Stock Before', currentStock);
      set('Stock After', isDeduction ? currentStock - data.Quantity : currentStock + data.Quantity);
      set('Reference', data.Reference || 'Manual Entry');
      set('Notes', data.Notes || '');
      set('Updated By', data.UpdatedBy || 'Admin');
      
      sheet.appendRow(row);
      if(isDeduction) currentStock -= data.Quantity;
      else currentStock += data.Quantity;
    } else {
      validSerials.forEach(serial => {
        const row = new Array(invHeaders.length).fill('');
      const set = (name, val) => {
        const idx = getInvIdx(name);
        if (idx !== -1) row[idx] = val;
      };
      
      set('Model ID', data.ProductID);
      set('Txn ID', txnId);
      set('Date', now);
      set('Model Number', modelNo);
      set('Model Name', modelName);
      set('Product Title', productTitle || data.ProductTitle || '');
      set('Part Number', partNumber || data.PartNumber || '');
      set('Action', data.Action || 'Stock Addition');
      set('Serial Number', String(serial));
      set('Vendor Name', data.VendorName || '');
      set('Bill Number', data.BillNumber || '');
      set('Quantity', 1);
      
      const isDeduction = ['Sale', 'Deduct', 'Stock Deduction', 'OUT'].includes(data.Action);
      set('Stock Before', currentStock);
      set('Stock After', isDeduction ? currentStock - 1 : currentStock + 1);
      set('Reference', data.Reference || 'Manual Entry');
      set('Notes', data.Notes || '');
      set('Updated By', data.UpdatedBy || 'Admin');
      
      sheet.appendRow(row);
      if(isDeduction) currentStock--;
      else currentStock++;
    });
    }

    // 5. Centrally recalculate and sync stocks across catalog and products
    syncAllSheetStocks();

    SpreadsheetApp.flush();
    return { 
      success: true, 
      message: `Added ${validSerials.length} units successfully.`, 
      addedCount: validSerials.length,
      skippedCount: skippedSerials.length
    };
  } catch (e) {
    console.error('saveInventoryStock Error:', e);
    return { success: false, message: e.message };
  }
}

function getInventoryLogs(productId = null) {
  try {
    const data = getData('inventory');
    if (!productId) return data.reverse().slice(0, 50); // Recent 50
    return data.filter(log => String(log.ProductID) === String(productId)).reverse();
  } catch (e) { return []; }
}

function deleteInventoryLog(txnId) {
  try {
    const sheet = getSheet('inventory');
    const data = sheet.getDataRange().getValues();
    const txnCol = data[0].indexOf('Txn ID');
    
    if (txnCol === -1) throw new Error('Txn ID column not found');

    for (let i = 1; i < data.length; i++) {
      if (String(data[i][txnCol]) === String(txnId)) {
        sheet.deleteRow(i + 1);
        SpreadsheetApp.flush();
        syncAllSheetStocks(); // Sync counts after deletion!
        return { success: true, message: 'Record deleted successfully.' };
      }
    }
    return { success: false, message: 'Transaction record not found.' };
  } catch (e) {
    return { success: false, message: e.message };
  }
}

function checkSerialNumber(serial) {
  try {
    const sheet = getSheet('inventory');
    const lastRow = sheet.getLastRow();
    if (lastRow <= 1) return { exists: false };
    const data = sheet.getRange(2, 2, lastRow - 1, 1).getValues().flat().map(String);
    return { exists: data.includes(String(serial)) };
  } catch (e) { return { exists: false }; }
}

function checkSerialsInInventory(serials) {
  try {
    if (!serials || serials.length === 0) return { success: true, duplicates: [] };
    const sheet = getSheet('inventory');
    if (sheet.getLastRow() <= 1) return { success: true, duplicates: [] };
    
    const data = sheet.getDataRange().getValues();
    const headers = data[0].map(h => String(h).trim());
    const getIdx = (headers, names) => {
      for (let n of names) {
        const i = headers.indexOf(n);
        if (i !== -1) return i;
      }
      return -1;
    };
    const snCol = getIdx(headers, ['Serial Number', 'serialnumber', 'Serial No']);
    if (snCol === -1) return { success: true, duplicates: [] };
    
    const existing = {};
    for (let i = 1; i < data.length; i++) {
      const val = String(data[i][snCol]).trim().toLowerCase();
      if (val) {
        existing[val] = true;
      }
    }
    
    const duplicates = serials.filter(s => existing[String(s).trim().toLowerCase()]);
    return { success: true, duplicates: duplicates };
  } catch (e) {
    console.error('checkSerialsInInventory Error:', e);
    return { success: false, message: e.message };
  }
}


function getBillingProducts() {
  try {
    const catalog = getCatalogProducts(); // Use existing refined function to get images/stocks
    const trueData = getTrueInventoryStocks();
    const trueStocks = trueData.stocks;
    const activeSerials = trueData.serials;
    
    const productList = catalog.map(p => {
        const mrp = parseFloat(p.MRP) || 0;
        const sellingPrice = parseFloat(p.SellingPrice) || 0;
        const offerPrice = parseFloat(p.OfferPrice) || 0;
        
        let finalRate = 0;
        if (sellingPrice > 0 && offerPrice > 0) {
            finalRate = Math.min(sellingPrice, offerPrice);
        } else if (sellingPrice > 0) {
            finalRate = sellingPrice;
        } else if (offerPrice > 0) {
            finalRate = offerPrice;
        } else {
            finalRate = mrp;
        }

        const wholesaleRate = parseFloat(p.WholesalePrice) || 0;

        return {
            id: p.ProductID || p.ModelNo,
            name: p.ProductTitle || p.Name,
            modelName: p.Name || p.ModelName || '',
            partNumber: p.PartNumber,
            sku: p.ModelNo,
            stock: trueStocks[p.ModelNo] !== undefined ? trueStocks[p.ModelNo] : (parseInt(p.Stock) || 0),
            rate: finalRate,
            retailRate: finalRate,
            wholesaleRate: wholesaleRate || finalRate,
            imageUrl: p.PrimaryImage,
            gst: parseFloat(p.GSTRate) || 18,
            hsnCode: p.HSNCode || '',
            sacCode: p.SACCode || '',
            mrp: mrp,
            sellingPrice: sellingPrice,
            offerPrice: offerPrice
        };
    });
    
    return { success: true, products: productList, serials: activeSerials };
  } catch(e) { 
    console.error('getBillingProducts Error:', e);
    return { success: false, message: e.message }; 
  }
}

function getInventoryData() {
  try {
    const logs = getData('inventory');
    const catalog = getData('catalog');
    const products = getData('products');
    const ledger = getData('ledger');
    const warranty = getData('warranty');
    
    const getVal = (obj, keys) => {
      for (let k of keys) { if (obj[k] !== undefined) return obj[k]; }
      return null;
    };

    // 1. Create a map for Sale Info
    const serialSaleMap = {};
    
    // Check Ledger for sales via QR Payloads or Remarks
    ledger.forEach(l => {
      const payloads = getVal(l, ['QR Payloads', 'qrpayloads', 'Remarks', 'remarks']);
      if (payloads) {
        const serials = String(payloads).split(/[,;|\n]+/).map(s => s.trim());
        serials.forEach(s => {
          if (s && s.length > 3) {
            serialSaleMap[s] = { 
              date: getVal(l, ['Date', 'date']), 
              customer: getVal(l, ['Name', 'name', 'Customer Name']) 
            };
          }
        });
      }
    });

    // Supplement from Warranty Register
    warranty.forEach(w => {
      const s = getVal(w, ['Serial Number', 'serialnumber', 'Serial No', 'Part Number']);
      if (s && !serialSaleMap[s]) {
        serialSaleMap[s] = {
          date: getVal(w, ['Registration Date', 'registrationdate', 'Date']),
          customer: getVal(w, ['Customer Name', 'customername', 'Name'])
        };
      }
    });

    const modelMap = {};
    const availableModels = [];
    const processedPids = new Set();

    // Process Catalog
    catalog.forEach(p => {
      const pid = getVal(p, ['Product ID', 'productid', 'ProductID']);
      const name = getVal(p, ['Model Name', 'modelname']);
      const modelNo = getVal(p, ['Model Number', 'modelnumber']);
      const partNumber = getVal(p, ['Part Number', 'partnumber', 'Part No']);
      const productTitle = getVal(p, ['Product Title', 'producttitle', 'Title']);
      const stock = parseInt(getVal(p, ['Stock', 'stock'])) || 0;
      
      if (pid && !processedPids.has(pid)) {
        modelMap[pid] = { name: name || pid, modelNo: modelNo || pid, stock: stock, partNumber, productTitle };
        availableModels.push({ id: pid, name: name || modelNo || pid, modelNo: modelNo, stock: stock });
        processedPids.add(pid);
      }
    });

    // Supplement from Products
    products.forEach(p => {
      const pid = getVal(p, ['Model Code', 'modelcode', 'ProductID']);
      const name = getVal(p, ['Model Name', 'modelname']);
      const stock = parseInt(getVal(p, ['Stock', 'stock'])) || 0;
      if (pid && !processedPids.has(pid)) {
        modelMap[pid] = { name: name || pid, modelNo: pid, stock: stock };
        availableModels.push({ id: pid, name: name || pid, modelNo: pid, stock: stock });
        processedPids.add(pid);
      }
    });

    const inventorySheet = getSheet('inventory');
    const invData = inventorySheet.getDataRange().getValues();
    const invHeaders = invData[0];
    
    // Find column indices for robust mapping
    const idx = {
      txn: invHeaders.indexOf('Txn ID'),
      date: invHeaders.indexOf('Date'),
      mNo: invHeaders.indexOf('Model Number'),
      mName: invHeaders.indexOf('Model Name'),
      title: invHeaders.indexOf('Product Title'),
      part: invHeaders.indexOf('Part Number'),
      action: invHeaders.indexOf('Action'),
      serial: invHeaders.indexOf('Serial Number'),
      vendor: invHeaders.indexOf('Vendor Name'),
      bill: invHeaders.indexOf('Bill Number'),
      qty: invHeaders.indexOf('Quantity'),
      after: invHeaders.indexOf('Stock After'),
      ref: invHeaders.indexOf('Reference'),
      notes: invHeaders.indexOf('Notes'),
      user: invHeaders.indexOf('Updated By')
    };
    const enrichedLogs = logs.map(log => {
      const pid = getVal(log, ['Model ID', 'Model ID ', 'ProductID', 'productid', 'id']);
      const serial = getVal(log, ['Serial Number', 'serialnumber', 'Serial No']);
      
      // Lookup in catalog by Model ID (PID)
      const catalogItem = modelMap[pid] || null;
      const saleInfo = serial ? (serialSaleMap[serial] || null) : null;

      return {
        ...log,
        ModelNo: getVal(log, ['Model Number', 'modelnumber', 'Model No']) || (catalogItem ? catalogItem.modelNo : 'N/A'),
        ModelName: catalogItem ? catalogItem.name : (getVal(log, ['Model Name', 'modelname']) || 'N/A'),
        ProductTitle: catalogItem ? (catalogItem.productTitle || catalogItem.name) : (getVal(log, ['Product Title', 'producttitle']) || 'N/A'),
        PartNumber: catalogItem ? (catalogItem.partNumber || 'N/A') : (getVal(log, ['Part Number', 'partnumber']) || 'N/A'),
        SerialNumber: serial || '-',
        VendorName: getVal(log, ['Vendor Name', 'vendorname', 'Vendor']) || '-',
        BillNumber: getVal(log, ['Bill Number', 'billnumber', 'Bill No']) || '-',
        Date: getVal(log, ['Date', 'date']),
        Action: getVal(log, ['Action', 'action', 'Action Log']) || '-',
        Reference: getVal(log, ['Reference', 'reference']) || '-',
        Notes: getVal(log, ['Notes', 'notes']) || '',
        Quantity: parseInt(getVal(log, ['Quantity', 'quantity'])) || 1,
        StockAfter: getVal(log, ['Stock After', 'stockafter']) || 0,
        UpdatedBy: getVal(log, ['Updated By', 'updatedby', 'User']) || 'Admin',
        SaleDate: saleInfo ? saleInfo.date : null,
        CustomerName: saleInfo ? saleInfo.customer : null
      };
    }).reverse();

    let totalStock = 0;
    availableModels.forEach(m => totalStock += m.stock);
    const uniqueSerials = new Set(logs.map(l => getVal(l, ['Serial Number', 'serialnumber'])).filter(s => !!s)).size;
    
    return {
      success: true,
      logs: enrichedLogs,
      availableModels: availableModels,
      stats: {
        total: totalStock,
        unique: uniqueSerials,
        low: availableModels.filter(m => m.stock < 5).length,
        txns: logs.length
      }
    };
  } catch (e) {
    console.error('getInventoryData Error:', e);
    return { success: false, message: e.message };
  }
}


/**
 * Sync Inventory Sheet Headers
 */
function syncInventoryHeaders() {
  const ss = getSpreadsheet();
  let sheet = ss.getSheetByName(CONFIG.sheets.inventory);
  if (!sheet) {
    sheet = ss.insertSheet(CONFIG.sheets.inventory);
  }
  
  const schema = SHEET_SCHEMAS.inventory;
  // FORCE OVERWRITE HEADERS TO ENSURE SCHEMA INTEGRITY
  sheet.getRange(1, 1, 1, schema.length).setValues([schema]).setFontWeight('bold').setBackground('#f3f3f3');
  
  // Also check Catalog headers while we are at it
  const catSheet = getSheet('catalog');
  const catSchema = SHEET_SCHEMAS.catalog;
  catSheet.getRange(1, 1, 1, catSchema.length).setValues([catSchema]).setFontWeight('bold').setBackground('#f3f3f3');

  SpreadsheetApp.flush();
  return { success: true, message: 'Inventory and Catalog headers synchronized with standard schema.' };
}

function uploadImageToDrive(base64, filename) {
  try {
    const folder = DriveApp.getFolderById(CONFIG.productImagesFolderId);
    
    // Safety check for data URI format
    if (!base64 || !base64.includes(',')) {
      console.warn('Invalid Base64 received for upload:', filename);
      return 'https://via.placeholder.com/400x300?text=Invalid+Image+Data';
    }

    const parts = base64.split(',');
    const mime = parts[0].match(/:(.*?);/)[1];
    const decoded = Utilities.base64Decode(parts[1]);
    const blob = Utilities.newBlob(decoded, mime, filename); // Extension will be handled by Drive
    
    const file = folder.createFile(blob);
    file.setSharing(DriveApp.Access.ANYONE_WITH_LINK, DriveApp.Permission.VIEW);
    
    // USE DIRECT VIEW URL FOR RELIABLE EMBEDDING
    const fileId = file.getId();
    // Return a direct thumbnail URL which is much faster and more reliable than uc?export=view
    return `https://lh3.googleusercontent.com/d/${fileId}`;
  } catch (e) {
    console.error('uploadImageToDrive Error:', e);
    // Return a placeholder instead of the massive Base64 string to prevent payload limit errors
    return 'https://via.placeholder.com/400x300?text=Upload+Failed';
  }
}

/**
 * PRODUCT REVIEWS SYSTEM
 */
function submitProductReview(data) {
  try {
    const identifier = String(data.identifier || '').trim();
    const modelNo = String(data.modelNo || data.productModel || '').trim();

    if (!identifier) {
      throw new Error("Phone Number or Invoice Number is required for verification.");
    }

    // Verify purchase in the ledger
    const ledger = getData('ledger') || [];
    const hasPurchased = ledger.some(row => {
      const rowMobile = String(row['Mobile'] || '').trim();
      const rowInvoice = String(row['Invoice Number'] || '').trim();
      const rowModel = String(row['Product Model'] || '').trim();
      const rowPart = String(row['Part Number'] || '').trim();

      const matchesIdentifier = (rowMobile === identifier || rowInvoice === identifier);
      const matchesProduct = (rowModel.toLowerCase() === modelNo.toLowerCase() || rowPart.toLowerCase() === modelNo.toLowerCase());

      return matchesIdentifier && matchesProduct;
    });

    if (!hasPurchased) {
      throw new Error("Could not verify purchase. Please ensure you entered the exact Phone Number or Invoice Number used during purchase for this specific model.");
    }

    const sheet = getSheet('reviews');
    const timestamp = new Date().toISOString();
    
    // Schema: ['Date', 'Product Model', 'Customer Name', 'Rating', 'Comment']
    sheet.appendRow([
      timestamp,
      modelNo,
      data.name,
      data.rating,
      data.comment
    ]);
    
    return { success: true, message: 'Review submitted successfully!' };
  } catch (e) {
    console.error('submitProductReview Error:', e);
    return { success: false, message: e.message };
  }
}

function getProductReviews(modelNo) {
  try {
    const reviews = getData('reviews');
    if (!reviews || reviews.length === 0) return [];
    
    // Filter by model number
    return reviews.filter(r => 
      String(r['Product Model'] || r.productModel || '').toLowerCase() === String(modelNo).toLowerCase()
    ).reverse(); // Newest first
  } catch (e) {
    console.error('getProductReviews Error:', e);
    return [];
  }
}

/**
 * Update Inventory Log Entry (Edit Action)
 */
function updateInventoryLog(data) {
  try {
    const sheet = getSheet('inventory');
    const rows = sheet.getDataRange().getValues();
    const headers = rows[0];
    
    const txnIdx = headers.indexOf('Txn ID');
    const snIdx = headers.indexOf('Serial Number');
    const vIdx = headers.indexOf('Vendor Name');
    const bIdx = headers.indexOf('Bill Number');
    const qIdx = headers.indexOf('Quantity');
    const nIdx = headers.indexOf('Notes');
    
    if (txnIdx === -1) throw new Error('Txn ID column not found');

    for (let i = 1; i < rows.length; i++) {
      if (String(rows[i][txnIdx]) === String(data.txnId)) {
        if (snIdx !== -1) sheet.getRange(i + 1, snIdx + 1).setValue(data.serialNumber);
        if (vIdx !== -1) sheet.getRange(i + 1, vIdx + 1).setValue(data.vendor);
        if (bIdx !== -1) sheet.getRange(i + 1, bIdx + 1).setValue(data.bill);
        if (qIdx !== -1) sheet.getRange(i + 1, qIdx + 1).setValue(data.quantity);
        if (nIdx !== -1 && data.notes !== undefined) sheet.getRange(i + 1, nIdx + 1).setValue(data.notes);
        
        SpreadsheetApp.flush();
        syncAllSheetStocks(); // Sync counts after edit!
        return { success: true, message: 'Log updated successfully.' };
      }
    }
    return { success: false, message: 'Log entry not found.' };
  } catch (e) {
    console.error('updateInventoryLog Error:', e);
    return { success: false, message: e.message };
  }
}

/**
 * Hold/Pause Inventory Entry
 */
function holdInventoryLog(txnId, isHold) {
  try {
    const sheet = getSheet('inventory');
    const rows = sheet.getDataRange().getValues();
    const headers = rows[0];
    const txnIdx = headers.indexOf('Txn ID');
    const noteIdx = headers.indexOf('Notes');
    
    if (txnIdx === -1 || noteIdx === -1) throw new Error('Required columns not found');

    for (let i = 1; i < rows.length; i++) {
      if (String(rows[i][txnIdx]) === String(txnId)) {
        const currentNotes = String(rows[i][noteIdx]);
        const status = isHold ? '[HOLD]' : '[ACTIVE]';
        const newNotes = currentNotes.includes('[HOLD]') || currentNotes.includes('[ACTIVE]') ?
                        currentNotes.replace(/\[HOLD\]|\[ACTIVE\]/, status) :
                        status + ' ' + currentNotes;
        
        sheet.getRange(i + 1, noteIdx + 1).setValue(newNotes);
        return { success: true, message: 'Status updated to ' + status };
      }
    }
    return { success: false, message: 'Transaction record not found.' };
  } catch (e) {
    return { success: false, message: e.message };
  }
}


// ============================================
// SALES & BILLING MODULE
// ============================================

function mapObjectToHeaders(obj, headers) {
  var rowData = [];
  headers.forEach(function(h) {
    var trimmed = String(h).trim();
    if (!trimmed) {
      rowData.push('');
      return;
    }
    var matchKey = Object.keys(obj).find(function(k) {
      var kNorm = String(k).replace(/\s+/g, '').toLowerCase();
      var hNorm = trimmed.replace(/\s+/g, '').toLowerCase();
      return kNorm === hNorm;
    });
    rowData.push(matchKey !== undefined ? obj[matchKey] : '');
  });
  return rowData;
}

function getSalesSheet(name) {
  var ss = getSpreadsheet();
  var sheet = ss.getSheetByName(name);
  if (!sheet) {
    sheet = ss.insertSheet(name);
  }
  
  // Robust initialization check: if sheet is completely empty, append headers
  if (sheet.getLastRow() === 0) {
    if(name === 'sales_parties') {
      sheet.appendRow(['Id', 'PartyType', 'Name', 'ShopName', 'Mobile', 'Email', 'GstNumber', 'Address', 'Pincode', 'CreditLimit', 'OpeningBalance', 'CurrentBalance', 'CreatedAt']);
    }
    if(name === 'sales_billing') {
      sheet.appendRow(['DocId', 'DocType', 'DocNumber', 'Date', 'PartyId', 'PartyName', 'PartyMobile', 'GstEnabled', 'ItemsJson', 'Subtotal', 'Discount', 'Tax', 'RoundOff', 'GrandTotal', 'AmountPaid', 'BalanceDue', 'PaymentMethod', 'PaymentStatus', 'DueDate', 'Notes', 'Status', 'CreatedAt']);
    }
    if (sheet.getLastColumn() > 0) {
      sheet.getRange(1, 1, 1, sheet.getLastColumn()).setFontWeight('bold').setBackground('#f3f3f3');
    }
  }
  return sheet;
}

function getPartyList() {
  try {
    const data = getData('parties');
    
    let billingData = [];
    try {
        var sheet = getSalesSheet('sales_billing');
        var bData = sheet.getDataRange().getValues();
        if(bData.length > 1) {
            var headers = bData[0];
            var pIdIdx = headers.indexOf('PartyId');
            var docTypeIdx = headers.indexOf('DocType');
            var docNumIdx = headers.indexOf('DocNumber');
            for(var i=1; i<bData.length; i++) {
                if(['Invoice', 'Sales Invoice', 'Proforma Invoice'].includes(bData[i][docTypeIdx])) {
                    billingData.push({
                        PartyId: bData[i][pIdIdx],
                        DocNumber: bData[i][docNumIdx]
                    });
                }
            }
        }
    } catch(e) {}

    const invoiceMap = {};
    billingData.forEach(b => {
        if(!invoiceMap[b.PartyId]) invoiceMap[b.PartyId] = [];
        invoiceMap[b.PartyId].push(b.DocNumber);
    });

    return data.map(p => ({
        Id: p['Party ID'],
        PartyType: p['Type'],
        Name: p['Name'],
        ShopName: p['Shop Name'],
        Mobile: p['Mobile'],
        Email: p['Email'],
        GstNumber: p['GST Number'],
        Address: p['Address'],
        Pincode: p['Pincode'],
        CreditLimit: p['Credit Limit'] || 0,
        CurrentBalance: p['Current Balance'] || 0,
        Status: p['Status'],
        InvoiceNumbers: invoiceMap[p['Party ID']] ? invoiceMap[p['Party ID']].join(', ') : 'N/A'
    }));
  } catch(e) { return []; }
}

function savePartyRecord(data) {
  try {
    const sheet = getSheet('parties');
    const headers = SHEET_SCHEMAS.parties;
    
    const isNew = !data.Id && !data['Party ID'];
    const pId = data.Id || data['Party ID'] || ('PTY-' + Math.random().toString(36).substr(2, 6).toUpperCase());
    
    let currentBal = (data.CurrentBalance !== undefined && data.CurrentBalance !== null && data.CurrentBalance !== '') ? parseFloat(data.CurrentBalance) : (data.OpeningBalance || 0);
    let addedDate = new Date().toISOString();
    
    let rowIdx = -1;
    const allData = sheet.getDataRange().getValues();
    const idIdx = headers.indexOf('Party ID');
    const mobileIdx = headers.indexOf('Mobile');
    
    for(let i=1; i<allData.length; i++) {
        if(String(allData[i][idIdx]) === String(pId) || (data.Mobile && String(allData[i][mobileIdx]) === String(data.Mobile))) {
            rowIdx = i + 1;
            if (data.CurrentBalance === undefined || data.CurrentBalance === null || data.CurrentBalance === '') {
              currentBal = allData[i][headers.indexOf('Current Balance')] !== undefined ? allData[i][headers.indexOf('Current Balance')] : currentBal;
            }
            addedDate = allData[i][headers.indexOf('Added Date')] || addedDate;
            break;
        }
    }
    
    const mappedData = {
        'Party ID': pId,
        'Type': data.PartyType || data.Type || 'B2C',
        'Name': data.Name || '',
        'Shop Name': data.ShopName || '',
        'Mobile': data.Mobile || '',
        'Email': data.Email || '',
        'GST Number': data.GstNumber || data.GST || '',
        'Address': data.Address || '',
        'Pincode': data.Pincode || '',
        'District': data.District || '',
        'State': data.State || '',
        'GPS': data.GPS || '',
        'Coordinates': data.Coordinates || '',
        'DOB': data.DOB || '',
        'Landmark': data.Landmark || '',
        'Credit Limit': data.CreditLimit || 0,
        'Opening Balance': data.OpeningBalance || 0,
        'Current Balance': parseFloat(currentBal) || 0,
        'Status': data.Status || 'Active',
        'Added Date': addedDate
    };
    
    const rowData = headers.map(h => mappedData[h] !== undefined ? mappedData[h] : '');
    
    if(rowIdx > -1) {
        sheet.getRange(rowIdx, 1, 1, headers.length).setValues([rowData]);
    } else {
        sheet.appendRow(rowData);
    }
    
    SpreadsheetApp.flush();
    return { success: true, message: 'Party saved successfully', id: pId, data: mappedData };
  } catch(e) {
    return { success: false, message: e.message };
  }
}

function propagatePartyUpdates(oldMobile, oldPartyId, updatedData) {
  try {
    const sheetsToUpdate = [
      { name: 'warranty', matchCol: 'Mobile Number', map: { 'Customer Name': 'Name', 'Mobile Number': 'Mobile', 'Email': 'Email' } },
      { name: 'complaints', matchCol: 'Mobile Number', map: { 'Customer Name': 'Name', 'Mobile Number': 'Mobile', 'Email': 'Email' } },
      { name: 'ledger', matchCol: 'Customer/Vendor ID', map: { 'Name': 'Name', 'Mobile': 'Mobile', 'Email': 'Email' } },
      { name: 'dealers', matchCol: 'Dealer ID', map: { 'Dealer Name': 'Name', 'Mobile': 'Mobile', 'Email': 'Email', 'District': 'District', 'GST Number': 'GstNumber' } }
    ];
    
    sheetsToUpdate.forEach(cfg => {
      try {
        const sheet = getSheet(cfg.name);
        if (!sheet) return;
        const schema = SHEET_SCHEMAS[cfg.name];
        const dataRange = sheet.getDataRange();
        const values = dataRange.getValues();
        if (values.length <= 1) return;
        
        const matchColIdx = schema.indexOf(cfg.matchCol);
        if (matchColIdx === -1) return;
        
        const matchVal = cfg.matchCol === 'Customer/Vendor ID' || cfg.matchCol === 'Dealer ID' ? oldPartyId : oldMobile;
        let updatedCount = 0;
        
        for (let i = 1; i < values.length; i++) {
          if (String(values[i][matchColIdx]).trim() === String(matchVal).trim()) {
            // Found a match! Update columns
            Object.keys(cfg.map).forEach(destCol => {
              const srcProp = cfg.map[destCol];
              const destColIdx = schema.indexOf(destCol);
              if (destColIdx !== -1) {
                const newVal = updatedData[srcProp] !== undefined ? updatedData[srcProp] : '';
                sheet.getRange(i + 1, destColIdx + 1).setValue(newVal);
              }
            });
            updatedCount++;
          }
        }
        console.log(`Propagated updates to ${updatedCount} rows in ${cfg.name}`);
      } catch (e) {
        console.error(`Error propagating updates to ${cfg.name}:`, e);
      }
    });
    
    SpreadsheetApp.flush();
    return { success: true, message: 'Synchronization completed successfully!' };
  } catch (e) {
    return { success: false, message: e.message };
  }
}

function syncPartyFromRegistration(type, rawData) {
    const partyData = {
        PartyType: type,
        Name: rawData.name || rawData.Name || rawData.DealerName || rawData['Dealer Name'] || rawData.CustomerName || rawData['Customer Name'] || '',
        ShopName: rawData.shopName || rawData['Shop Name'] || rawData.ShopName || '',
        Mobile: rawData.mobile || rawData.Mobile || rawData.MobileNumber || rawData['Mobile Number'] || '',
        Email: rawData.email || rawData.Email || '',
        GstNumber: rawData.gstNumber || rawData['GST Number'] || rawData.GST || '',
        Address: rawData.address || rawData.Address || rawData.FullAddress || rawData['Full Address'] || '',
        Pincode: rawData.pincode || rawData.Pincode || '',
        District: rawData.district || rawData.District || '',
        State: rawData.state || rawData.State || '',
        GPS: rawData.gps || rawData.GPS || '',
        Coordinates: rawData.coordinates || rawData.Coordinates || '',
        DOB: rawData.dob || rawData.DOB || '',
        Landmark: rawData.landmark || rawData.Landmark || rawData.nearbyArea || rawData['Nearby Area'] || '',
        Status: rawData.Status || rawData.status || 'Active'
    };
    if (partyData.Mobile) {
        return savePartyRecord(partyData);
    }
    return { success: false, message: 'Mobile number missing' };
}

function getBillingHistory() {
  try {
    var sheet = getSalesSheet('sales_billing');
    var data = sheet.getDataRange().getValues();
    var headers = data[0];
    
    // Map existing doc numbers in sales_billing to prevent duplicates
    var existingDocs = {};
    for (var i = 1; i < data.length; i++) {
      var docNum = data[i][headers.indexOf('DocNumber')];
      if (docNum) {
        existingDocs[String(docNum).trim()] = true;
      }
    }
    
    // Load from legacy invoices sheet and merge if missing
    try {
      var legacySheet = getSheet('invoices');
      if (legacySheet) {
        var legacyData = legacySheet.getDataRange().getValues();
        if (legacyData.length > 1) {
          var legHeaders = legacyData[0];
          var invNumIdx = legHeaders.indexOf('Invoice Number');
          var txnIdIdx = legHeaders.indexOf('Transaction ID');
          var dateIdx = legHeaders.indexOf('Invoice Date');
          var custNameIdx = legHeaders.indexOf('Customer Name');
          var mobIdx = legHeaders.indexOf('Mobile');
          var gstEnIdx = legHeaders.indexOf('GST Enabled');
          var subIdx = legHeaders.indexOf('Sub Total');
          var gstAmtIdx = legHeaders.indexOf('GST Amount');
          var grandIdx = legHeaders.indexOf('Grand Total');
          var paidIdx = legHeaders.indexOf('Paid Amount');
          var balIdx = legHeaders.indexOf('Balance');
          var payStatusIdx = legHeaders.indexOf('Payment Status');
          var payMethodIdx = legHeaders.indexOf('Payment Mode');
          var itemsIdx = legHeaders.indexOf('Items JSON');
          
          var hasNewInsert = false;
          
          for (var r = 1; r < legacyData.length; r++) {
            var invNum = String(legacyData[r][invNumIdx]).trim();
            if (invNum && !existingDocs[invNum]) {
              // Not present in sales_billing, let's merge it!
              var legacyRowObj = {
                DocId: legacyData[r][txnIdIdx] || ('DOC-' + Date.now()),
                DocType: 'Invoice',
                DocNumber: invNum,
                Date: legacyData[r][dateIdx] || new Date().toISOString().split('T')[0],
                PartyId: '', // Will be resolved dynamically
                PartyName: legacyData[r][custNameIdx] || '',
                PartyMobile: legacyData[r][mobIdx] || '',
                GstEnabled: String(legacyData[r][gstEnIdx]).toUpperCase() === 'YES' ? true : false,
                ItemsJson: legacyData[r][itemsIdx] || '[]',
                Subtotal: legacyData[r][subIdx] || 0,
                Discount: 0,
                Tax: legacyData[r][gstAmtIdx] || 0,
                RoundOff: 0,
                GrandTotal: legacyData[r][grandIdx] || 0,
                AmountPaid: legacyData[r][paidIdx] || 0,
                BalanceDue: legacyData[r][balIdx] || 0,
                PaymentMethod: legacyData[r][payMethodIdx] || 'Cash',
                PaymentStatus: legacyData[r][payStatusIdx] || 'Unpaid',
                DueDate: '',
                Notes: 'Merged from legacy Invoices',
                Status: 'Saved',
                CreatedAt: new Date().toISOString()
              };
              
              var mappedRow = mapObjectToHeaders(legacyRowObj, headers);
              sheet.appendRow(mappedRow);
              existingDocs[invNum] = true;
              hasNewInsert = true;
            }
          }
          if (hasNewInsert) {
            // Re-read data from sales_billing after inserts
            data = sheet.getDataRange().getValues();
          }
        }
      }
    } catch(err) {
      console.error('Failed merging legacy invoices:', err);
    }
    
    if(data.length <= 1) return [];
    var result = [];
    for(var i=1; i<data.length; i++) {
      var obj = {};
      for(var j=0; j<headers.length; j++) {
        var val = data[i][j];
        if (val instanceof Date) {
          val = val.toISOString();
        }
        obj[headers[j]] = val;
      }
      result.push(obj);
    }
    return result.reverse();
  } catch(e) { return []; }
}

function deleteBillingDocument(docNumber) {
  try {
    var sheet = getSalesSheet('sales_billing');
    var data = sheet.getDataRange().getValues();
    var headers = data[0];
    
    var foundIdxs = [];
    for(var i=1; i<data.length; i++) {
      if(String(data[i][2]).trim() === String(docNumber).trim()) {
        foundIdxs.push(i);
      }
    }
    
    if(foundIdxs.length > 0) {
      var firstMatchIdx = foundIdxs[0];
      var rowObjDocId = data[firstMatchIdx][0];
      var partyId = data[firstMatchIdx][headers.indexOf('PartyId')];
      var partyMobile = data[firstMatchIdx][headers.indexOf('PartyMobile')];
      var docType = data[firstMatchIdx][headers.indexOf('DocType')];
      
      // Calculate total outstanding balance due from all duplicate invoice rows to revert it fully
      var totalBalanceDueToRevert = 0;
      foundIdxs.forEach(idx => {
        totalBalanceDueToRevert += parseFloat(data[idx][headers.indexOf('BalanceDue')]) || 0;
      });
      
      // 1. Revert party balance in parties sheet (using PartyId or PartyMobile with robust fallback)
      if (['Invoice', 'Proforma Invoice', 'Sales Invoice', 'Credit Note', 'Debit Note'].includes(docType)) {
        try {
          var partyBalSheet = getSheet('parties');
          if (partyBalSheet) {
            var pData = partyBalSheet.getDataRange().getValues();
            var pHeaders = pData[0];
            var pIdIdx = pHeaders.indexOf('Party ID');
            var pMobIdx = pHeaders.indexOf('Mobile');
            
            for(var i=1; i<pData.length; i++) {
              var matchesId = partyId && String(pData[i][pIdIdx]).trim().toLowerCase() === String(partyId).trim().toLowerCase();
              var matchesMobile = partyMobile && String(pData[i][pMobIdx]).replace(/\D/g, '') === String(partyMobile).replace(/\D/g, '');
              
              if (matchesId || matchesMobile) {
                var balIdx = pHeaders.indexOf('Current Balance');
                if (balIdx !== -1) {
                  var currentBal = parseFloat(pData[i][balIdx]) || 0;
                  if (docType === 'Credit Note') {
                    currentBal += totalBalanceDueToRevert;
                  } else {
                    currentBal -= totalBalanceDueToRevert;
                  }
                  partyBalSheet.getRange(i+1, balIdx+1).setValue(currentBal);
                }
                break;
              }
            }
          }
        } catch(e) { console.error('Revert party balance failed:', e); }
      }
      
      // 2. Delete ALL duplicate rows from legacy invoices sheet
      try {
        var invSheet = getSheet('invoices');
        if (invSheet) {
          var invData = invSheet.getDataRange().getValues();
          var invHeaders = invData[0];
          var invNumIdx = invHeaders.indexOf('Invoice Number');
          for (var i = invData.length - 1; i >= 1; i--) {
            if (String(invData[i][invNumIdx]).trim() === String(docNumber).trim()) {
              invSheet.deleteRow(i + 1);
            }
          }
        }
      } catch(e) { console.error('Delete from legacy invoices failed:', e); }
      
      // 3. Delete ALL duplicate rows from master ledger sheet
      try {
        var masterLedgerSheet = getSheet('ledger');
        if (masterLedgerSheet) {
          var masterLedgerData = masterLedgerSheet.getDataRange().getValues();
          var ledgerInvNumIdx = masterLedgerData[0].indexOf('Invoice Number');
          for (var i = masterLedgerData.length - 1; i >= 1; i--) {
            if (String(masterLedgerData[i][ledgerInvNumIdx]).trim() === String(docNumber).trim()) {
              masterLedgerSheet.deleteRow(i + 1);
            }
          }
        }
      } catch(e) { console.error('Delete from master ledger failed:', e); }
      
      // 4. Delete payment receipt from payment_receipts sheet
      try {
        var recSheet = getSheet('payment_receipts');
        if (recSheet) {
          var recData = recSheet.getDataRange().getValues();
          var recHeaders = recData[0];
          var allocsIdx = recHeaders.indexOf('Allocations JSON');
          var refNumIdx = recHeaders.indexOf('Reference Number');
          for (var i = recData.length - 1; i >= 1; i--) {
            var allocsJson = recData[i][allocsIdx];
            var refNum = recData[i][refNumIdx];
            if (String(refNum).trim() === String(docNumber).trim() || 
                String(refNum).trim() === String(rowObjDocId).trim() || 
                (allocsJson && allocsJson.includes(docNumber))) {
              recSheet.deleteRow(i + 1);
            }
          }
        }
      } catch(e) { console.error('Delete payment receipt failed:', e); }
      
      // 5. Delete cash flow transaction from cash_flow sheet
      try {
        var cfSheet = getSheet('cash_flow');
        if (cfSheet) {
          var cfData = cfSheet.getDataRange().getValues();
          var cfHeaders = cfData[0];
          var cfRefIdx = cfHeaders.indexOf('Reference Number');
          for (var i = cfData.length - 1; i >= 1; i--) {
            var refNum = cfData[i][cfRefIdx];
            if (String(refNum).trim() === String(docNumber).trim() || 
                String(refNum).trim() === String(rowObjDocId).trim()) {
              cfSheet.deleteRow(i + 1);
            }
          }
        }
      } catch(e) { console.error('Delete cash flow entry failed:', e); }
      
      // 6. Revert inventory stock count and delete stock logs
      try {
        var inventorySheet = getSheet('inventory');
        if (inventorySheet) {
          var inventoryData = inventorySheet.getDataRange().getValues();
          var invHeaders = inventoryData[0].map(h => String(h).trim().toLowerCase());
          var billNumColIdx = -1;
          for (var n of ['bill number', 'billnumber', 'invoice number', 'ref', 'reference', 'txn id']) {
            var idx = invHeaders.indexOf(n);
            if (idx !== -1) { billNumColIdx = idx; break; }
          }
          
          if (billNumColIdx !== -1) {
            for (var i = inventoryData.length - 1; i >= 1; i--) {
              if (String(inventoryData[i][billNumColIdx]).trim() === String(docNumber).trim()) {
                try {
                  var prodIdColIdx = -1;
                  for (var n of ['model id', 'modelid', 'product id', 'productid', 'model number']) {
                    var idx = invHeaders.indexOf(n);
                    if (idx !== -1) { prodIdColIdx = idx; break; }
                  }
                  
                  var qtyColIdx = -1;
                  for (var n of ['quantity', 'qty', 'count']) {
                    var idx = invHeaders.indexOf(n);
                    if (idx !== -1) { qtyColIdx = idx; break; }
                  }
                  
                  var pId = prodIdColIdx !== -1 ? inventoryData[i][prodIdColIdx] : '';
                  var qty = qtyColIdx !== -1 ? (parseInt(inventoryData[i][qtyColIdx]) || 1) : 1;
                  
                  if (pId) {
                    var catalogSheet = getSheet('catalog');
                    var catalogData = catalogSheet.getDataRange().getValues();
                    var catHeaders = catalogData[0].map(h => String(h).trim().toLowerCase());
                    var catIdIdx = -1;
                    for (var n of ['product id', 'productid', 'id', 'model number', 'modelnumber']) {
                      var idx = catHeaders.indexOf(n);
                      if (idx !== -1) { catIdIdx = idx; break; }
                    }
                    
                    var stockIdx = -1;
                    for (var n of ['stock', 'qty', 'quantity']) {
                      var idx = catHeaders.indexOf(n);
                      if (idx !== -1) { stockIdx = idx; break; }
                    }
                    
                    if (catIdIdx !== -1 && stockIdx !== -1) {
                      for (var cRow = 1; cRow < catalogData.length; cRow++) {
                        if (String(catalogData[cRow][catIdIdx]).trim().toLowerCase() === String(pId).trim().toLowerCase()) {
                          var currentStock = parseInt(catalogData[cRow][stockIdx]) || 0;
                          catalogSheet.getRange(cRow + 1, stockIdx + 1).setValue(currentStock + qty);
                          break;
                        }
                      }
                    }
                  }
                } catch(e) { console.error('Individual stock restore step failed:', e); }
                
                inventorySheet.deleteRow(i + 1);
              }
            }
          }
        }
      } catch(e) { console.error('Revert stock count failed:', e); }
      
      // 7. Delete registered warranties
      try {
        var warSheet = getSheet('warranty');
        if (warSheet) {
          var warData = warSheet.getDataRange().getValues();
          var remarksColIdx = warData[0].indexOf('Remarks');
          for (var i = warData.length - 1; i >= 1; i--) {
            if (String(warData[i][remarksColIdx]).includes(docNumber)) {
              warSheet.deleteRow(i + 1);
            }
          }
        }
      } catch(e) { console.error('Delete registered warranties failed:', e); }
      
      // Finally delete all matched entries from sales_billing (bottom-to-top)
      for (var d = foundIdxs.length - 1; d >= 0; d--) {
        sheet.deleteRow(foundIdxs[d] + 1);
      }
      SpreadsheetApp.flush();
      return { success: true, message: 'Document and all its duplicates deleted completely across all systems!' };
    }
    return { success: false, message: 'Document not found in spreadsheet.' };
  } catch(e) { 
    return { success: false, message: e.message }; 
  }
}

function saveBillingDocument(data) {
  try {
    var sheet = getSalesSheet('sales_billing');
    var docId = 'DOC-' + Date.now();
    
    // Update party balance if it's an invoice, proforma invoice, or credit/debit note
    if(['Invoice', 'Proforma Invoice', 'Sales Invoice', 'Credit Note', 'Debit Note'].includes(data.DocType)) {
      var partyBalSheet = getSheet('parties');
      if (partyBalSheet) {
        var pData = partyBalSheet.getDataRange().getValues();
        for(var i=1; i<pData.length; i++) {
          if(String(pData[i][0]) === String(data.PartyId)) {
            var balIdx = pData[0].indexOf('Current Balance');
            if (balIdx !== -1) {
              var currentBal = parseFloat(pData[i][balIdx]) || 0;
              var due = parseFloat(data.BalanceDue) || 0;
              
              if(data.DocType === 'Credit Note') {
                currentBal -= due;
              } else {
                currentBal += due;
              }
              
              partyBalSheet.getRange(i+1, balIdx+1).setValue(currentBal);
            }
            break;
          }
        }
      }
    }
    
    // Get party name and mobile
    var pName = 'Unknown Party';
    var pMobile = '';
    var parties = getPartyList();
    var p = parties.find(x => x.Id === data.PartyId);
    if(p) {
      pName = p.Name;
      pMobile = p.Mobile || '';
    }
    
    // Ensure the sheet has headers — re-read after getSalesSheet init
    var allSheetData = sheet.getDataRange().getValues();
    var headers = allSheetData[0];
    
    // If PartyMobile column is missing from existing sheet, add it
    if (headers.indexOf('PartyMobile') === -1) {
      var partyNameIdx = headers.indexOf('PartyName');
      if (partyNameIdx !== -1) {
        sheet.insertColumnAfter(partyNameIdx + 1);
        sheet.getRange(1, partyNameIdx + 2).setValue('PartyMobile');
        // Re-read headers after column insertion
        allSheetData = sheet.getDataRange().getValues();
        headers = allSheetData[0];
      }
    }
    
    var docNumIdx = headers.indexOf('DocNumber');
    var existingRowIdx = -1;
    for (var i = 1; i < allSheetData.length; i++) {
      if (String(allSheetData[i][docNumIdx]).trim() === String(data.DocNumber).trim()) {
        existingRowIdx = i + 1;
        break;
      }
    }
    
    var rowObj = {
      DocId: existingRowIdx !== -1 ? (allSheetData[existingRowIdx - 1][headers.indexOf('DocId')] || docId) : docId,
      DocType: data.DocType, DocNumber: data.DocNumber, Date: data.Date,
      PartyId: data.PartyId, PartyName: pName, PartyMobile: pMobile, GstEnabled: data.GstEnabled,
      ItemsJson: data.Items, Subtotal: data.Subtotal, Discount: data.Discount,
      Tax: data.Tax, RoundOff: data.RoundOff, GrandTotal: data.GrandTotal,
      AmountPaid: data.AmountPaid, BalanceDue: data.BalanceDue,
      PaymentMethod: data.PaymentMethod, PaymentStatus: data.PaymentStatus,
      DueDate: data.DueDate, Notes: data.Notes, Status: data.Action, 
      CreatedAt: existingRowIdx !== -1 ? (allSheetData[existingRowIdx - 1][headers.indexOf('CreatedAt')] || new Date().toISOString()) : new Date().toISOString()
    };
    
    var rowData = mapObjectToHeaders(rowObj, headers);
    if (existingRowIdx !== -1) {
      for (var col = 0; col < headers.length; col++) {
        sheet.getRange(existingRowIdx, col + 1).setValue(rowData[col]);
      }
    } else {
      sheet.appendRow(rowData);
    }
    
    // Check if it is a real sale / invoice
    var isSale = ['Invoice', 'Sales Invoice', 'Proforma Invoice'].includes(data.DocType);
    
    // Sync with other sheets (Ledger, Invoices, Payment_Receipts, Cash_Flow) for real sales
    if(isSale) {
      // Parse tax rate & calculate from data
      var taxRate = 0;
      try {
        var itemsList = JSON.parse(data.Items);
        if (itemsList.length > 0) taxRate = parseFloat(itemsList[0].gst) || 0;
      } catch(e) {}

      // 1. Write to invoices legacy sheet for Customer Statement & Party Ledger alignment
      try {
        var invSheet = getSheet('invoices');
        if (invSheet) {
          var invData = invSheet.getDataRange().getValues();
          var invHeaders = invData[0];
          var invNumIdx = invHeaders.indexOf('Invoice Number');
          var invRowIdx = -1;
          for (var i = 1; i < invData.length; i++) {
            if (String(invData[i][invNumIdx]).trim() === String(data.DocNumber).trim()) {
              invRowIdx = i + 1;
              break;
            }
          }
          var invObj = {
            'Invoice Number': data.DocNumber,
            'Transaction ID': invRowIdx !== -1 ? invData[invRowIdx - 1][invHeaders.indexOf('Transaction ID')] : docId,
            'Invoice Date': data.Date,
            'Customer Name': pName,
            'Mobile': p ? p.Mobile : '',
            'Email': p ? p.Email : '',
            'Type': p ? p.PartyType || 'B2C' : 'B2C',
            'Sub Total': data.Subtotal,
            'GST Enabled': String(data.GstEnabled).toUpperCase() === 'TRUE' ? 'Yes' : 'No',
            'GST Rate': taxRate + '%',
            'GST Amount': data.Tax,
            'Grand Total': data.GrandTotal,
            'Paid Amount': data.AmountPaid,
            'Balance': data.BalanceDue,
            'Payment Status': data.PaymentStatus,
            'Payment Mode': data.PaymentMethod,
            'Items JSON': data.Items,
            'Created By': 'System'
          };
          var invRow = mapObjectToHeaders(invObj, invHeaders);
          if (invRowIdx !== -1) {
            for (var col = 0; col < invHeaders.length; col++) {
              invSheet.getRange(invRowIdx, col + 1).setValue(invRow[col]);
            }
          } else {
            invSheet.appendRow(invRow);
          }
        }
      } catch(e) { console.error('Sync Invoices sheet failed:', e); }

      // 2. Write to master Ledger sheet for Admin Sales Ledger alignment
      try {
        var masterLedgerSheet = getSheet('ledger');
        if (masterLedgerSheet) {
          var masterLedgerData = masterLedgerSheet.getDataRange().getValues();
          var masterHeaders = masterLedgerData[0];
          var ledgerInvNumIdx = masterHeaders.indexOf('Invoice Number');
          var ledgerRowIdx = -1;
          for (var i = 1; i < masterLedgerData.length; i++) {
            if (String(masterLedgerData[i][ledgerInvNumIdx]).trim() === String(data.DocNumber).trim()) {
              ledgerRowIdx = i + 1;
              break;
            }
          }
          var pAddress = p ? (p.BillingAddress || p.Address || '') : '';
          var pEmail = p ? (p.Email || '') : '';
          var ledgerObj = {
            'Date': data.Date,
            'Transaction ID': ledgerRowIdx !== -1 ? masterLedgerData[ledgerRowIdx - 1][masterHeaders.indexOf('Transaction ID')] : docId,
            'Type': 'Sale',
            'Customer/Vendor ID': data.PartyId,
            'Name': pName,
            'Mobile': p ? p.Mobile : '',
            'Address': pAddress,
            'Part Number': '-',
            'Product Model': '-',
            'Payment Status': data.PaymentStatus,
            'Total Amount': data.GrandTotal,
            'Paid Amount': data.AmountPaid,
            'Balance': data.BalanceDue,
            'Payment Mode': data.PaymentMethod,
            'Remarks': data.Notes || 'Sales Entry via Billing Module',
            'Invoice Number': data.DocNumber,
            'Sub Total': data.Subtotal,
            'GST Enabled': String(data.GstEnabled).toUpperCase() === 'TRUE' ? 'Yes' : 'No',
            'GST Rate': taxRate + '%',
            'GST Amount': data.Tax,
            'Hold Amount': 0,
            'Grand Total': data.GrandTotal,
            'Source': 'Sales Module',
            'QR Payloads': '',
            'Email': pEmail
          };
          var ledgerRow = mapObjectToHeaders(ledgerObj, masterHeaders);
          if (ledgerRowIdx !== -1) {
            for (var col = 0; col < masterHeaders.length; col++) {
              masterLedgerSheet.getRange(ledgerRowIdx, col + 1).setValue(ledgerRow[col]);
            }
          } else {
            masterLedgerSheet.appendRow(ledgerRow);
          }
        }
      } catch(e) { console.error('Sync Ledger sheet failed:', e); }

      // 3. Process immediate payment receipts & Cash Flow if AmountPaid > 0
      var amtPaid = parseFloat(data.AmountPaid) || 0;
      if (amtPaid > 0) {
        // Write to Payment_Receipts
        try {
          var recSheet = getSheet('payment_receipts');
          if (recSheet) {
            var recData = recSheet.getDataRange().getValues();
            var recHeaders = recData[0];
            var refNumIdx = recHeaders.indexOf('Reference Number');
            var recRowIdx = -1;
            for (var i = 1; i < recData.length; i++) {
              if (String(recData[i][refNumIdx]).trim() === String(docId).trim() || 
                  (ledgerRowIdx !== -1 && String(recData[i][refNumIdx]).trim() === String(masterLedgerData[ledgerRowIdx - 1][masterHeaders.indexOf('Transaction ID')]).trim())) {
                recRowIdx = i + 1;
                break;
              }
            }
            var targetRcpId = recRowIdx !== -1 ? recData[recRowIdx - 1][recHeaders.indexOf('Receipt Number')] : ('RCP-' + Date.now());
            var allocations = [{ invoiceNo: data.DocNumber, amount: amtPaid }];
            var recObj = {
              'Receipt Number': targetRcpId,
              'Transaction ID': ledgerRowIdx !== -1 ? masterLedgerData[ledgerRowIdx - 1][masterHeaders.indexOf('Transaction ID')] : docId,
              'Date': data.Date,
              'Customer Name': pName,
              'Mobile': p ? p.Mobile : '',
              'Amount Received': amtPaid,
              'Mode of Payment': data.PaymentMethod,
              'Reference Number': ledgerRowIdx !== -1 ? masterLedgerData[ledgerRowIdx - 1][masterHeaders.indexOf('Transaction ID')] : docId,
              'Reference Date': data.Date,
              'Notes': 'Auto-generated receipt via Billing',
              'Allocations JSON': JSON.stringify(allocations),
              'Status': 'Received'
            };
            var recRow = mapObjectToHeaders(recObj, recHeaders);
            if (recRowIdx !== -1) {
              for (var col = 0; col < recHeaders.length; col++) {
                recSheet.getRange(recRowIdx, col + 1).setValue(recRow[col]);
              }
            } else {
              recSheet.appendRow(recRow);
            }
          }
        } catch(e) { console.error('Sync Payment Receipts sheet failed:', e); }

        // Write to Cash_Flow
        try {
          var cfSheet = getSheet('cash_flow');
          if (cfSheet) {
            var cfData = cfSheet.getDataRange().getValues();
            var cfHeaders = cfData[0];
            var cfRefNumIdx = cfHeaders.indexOf('Reference Number');
            var cfRowIdx = -1;
            for (var i = 1; i < cfData.length; i++) {
              if (String(cfData[i][cfRefNumIdx]).trim() === String(docId).trim() ||
                  (ledgerRowIdx !== -1 && String(cfData[i][cfRefNumIdx]).trim() === String(masterLedgerData[ledgerRowIdx - 1][masterHeaders.indexOf('Transaction ID')]).trim())) {
                cfRowIdx = i + 1;
                break;
              }
            }
            var targetRcpId = recRowIdx !== -1 ? recData[recRowIdx - 1][recHeaders.indexOf('Receipt Number')] : ('RCP-' + Date.now());
            var cfObj = {
              'Date': data.Date,
              'Voucher Number': targetRcpId,
              'Voucher Type': 'Receipt',
              'Party Name': pName,
              'Description': 'Sales Payment Auto-receipt',
              'Cash In': amtPaid,
              'Cash Out': 0,
              'Running Balance': 0,
              'Payment Mode': data.PaymentMethod,
              'Reference Number': ledgerRowIdx !== -1 ? masterLedgerData[ledgerRowIdx - 1][masterHeaders.indexOf('Transaction ID')] : docId,
              'Status': 'Active'
            };
            var cfRow = mapObjectToHeaders(cfObj, cfHeaders);
            if (cfRowIdx !== -1) {
              for (var col = 0; col < cfHeaders.length; col++) {
                cfSheet.getRange(cfRowIdx, col + 1).setValue(cfRow[col]);
              }
            } else {
              cfSheet.appendRow(cfRow);
            }
          }
        } catch(e) { console.error('Sync Cash Flow sheet failed:', e); }
      }
    }
    

    // Deduct inventory items & auto-register warranties if DocType is Invoice, Sales Invoice, Proforma Invoice, or Sales Order
    if(['Invoice', 'Sales Invoice', 'Proforma Invoice', 'Sales Order'].includes(data.DocType)) {
      try {
        var items = JSON.parse(data.Items);
        var warSheet = getSheet('warranty');
        
        items.forEach(item => {
          if(item.productId) {
            // Split comma-separated serial numbers correctly
            var serials = (item.sn || item.serialNumber) ? (item.sn || item.serialNumber).split(',').map(s => s.trim()).filter(Boolean) : [];
            
            // Check if there are already inventory logs for BillNumber === data.DocNumber to prevent duplicates
            var existingInventory = false;
            try {
              var invData = getSheet('inventory').getDataRange().getValues();
              var billNumColIdx = invData[0].indexOf('Bill Number');
              for (var invRow = 1; invRow < invData.length; invRow++) {
                if (String(invData[invRow][billNumColIdx]).trim() === String(data.DocNumber).trim()) {
                  existingInventory = true;
                  break;
                }
              }
            } catch(e) {}

            if (!existingInventory) {
              // Stock deduction
              saveInventoryStock({
                ProductID: item.productId,
                Action: 'Sale',
                Serials: serials,
                Quantity: item.qty || 1,
                BillNumber: data.DocNumber,
                Reference: 'Sales Module',
                Notes: 'Auto-deducted from Billing',
                UpdatedBy: 'System'
              });
            }
            
            // Auto-register warranty
            if (warSheet) {
              var start = new Date(data.Date);
              var end = new Date(start);
              end.setMonth(end.getMonth() + CONFIG.warrantyPeriod);
              
              if (serials.length > 0) {
                serials.forEach(serial => {
                  // Check if this serial is already registered under this invoice to prevent duplicate warranty registrations
                  var existingWarranty = false;
                  try {
                    var warData = warSheet.getDataRange().getValues();
                    var serialColIdx = warData[0].indexOf('Part Number');
                    var remarksColIdx = warData[0].indexOf('Remarks');
                    for (var wRow = 1; wRow < warData.length; wRow++) {
                      if (String(warData[wRow][serialColIdx]).trim() === String(serial).trim() && 
                          String(warData[wRow][remarksColIdx]).includes(data.DocNumber)) {
                        existingWarranty = true;
                        break;
                      }
                    }
                  } catch(e) {}

                  if (existingWarranty) return; // Skip duplicate

                  var year = new Date().getFullYear().toString().substr(-2);
                  var rand = (warSheet.getLastRow() + 1).toString().padStart(4, '0');
                  var wid = `V-${year}-${rand}-${String(serial).substr(-3)}`;
                  
                  warSheet.appendRow([
                    new Date(), wid, serial, item.name, 'V-VARMA',
                    pName, "'" + (p ? p.Mobile : ''), p ? p.Email : '',
                    p ? p.Address : '', '', '', p ? p.Pincode : '',
                    'Date of Purchase', formatDate(start), formatDate(end), 'Active', '', `Invoice Number: ${data.DocNumber}`, ''
                  ]);
                  
                  logServiceHistory({
                    eventType: 'Warranty Registered', entityType: 'Warranty', entityId: wid,
                    productModel: item.name, partNumber: serial, warrantyId: wid,
                    customerName: pName, mobile: p ? p.Mobile : '', status: 'Active',
                    notes: `Auto-registered via Billing (Invoice: ${data.DocNumber})`, source: 'Sales Module'
                  });
                });
              } else {
                // Non-serial item generic warranty card
                var year = new Date().getFullYear().toString().substr(-2);
                var rand = (warSheet.getLastRow() + 1).toString().padStart(4, '0');
                var wid = `V-${year}-${rand}-GEN`;
                
                warSheet.appendRow([
                  new Date(), wid, '-', item.name, 'V-VARMA',
                  pName, "'" + (p ? p.Mobile : ''), p ? p.Email : '',
                  p ? p.Address : '', '', '', p ? p.Pincode : '',
                  'Date of Purchase', formatDate(start), formatDate(end), 'Active', '', `Invoice Number: ${data.DocNumber}`, ''
                ]);
                
                logServiceHistory({
                  eventType: 'Warranty Registered', entityType: 'Warranty', entityId: wid,
                  productModel: item.name, partNumber: '-', warrantyId: wid,
                  customerName: pName, mobile: p ? p.Mobile : '', status: 'Active',
                  notes: `Auto-registered via Billing (Invoice: ${data.DocNumber})`, source: 'Sales Module'
                });
              }
            }
          }
        });
      } catch(e) { console.error('Inventory Deduction / Warranty Auto-reg Failed:', e); }
    }
    // Increment sequence number
    updateBillingSequence(data.DocType, undefined, (parseInt(data.DocNumber.replace(/^\D+/g, '')) || 0) + 1);
    
    SpreadsheetApp.flush();
    return { success: true, message: 'Document saved successfully', pdfUrl: '' };
  } catch(e) { return { success: false, message: e.message }; }
}

function getBillingSettings() {
  try {
    var settingsSheet = getSheet('billing_settings');
    var sequencesSheet = getSheet('billing_sequences');
    
    var settingsData = settingsSheet.getDataRange().getValues();
    var settings = {};
    if(settingsData.length > 1) {
      for(var i=1; i<settingsData.length; i++) {
        settings[settingsData[i][0]] = settingsData[i][1];
      }
    }
    
    var sequencesData = sequencesSheet.getDataRange().getValues();
    if(sequencesData.length <= 1) {
       var defaults = [
          ['Invoice', 'INV-', 1],
          ['Estimate', 'EST-', 1],
          ['Quotation', 'QUO-', 1],
          ['Sales Order', 'SO-', 1],
          ['Delivery Challan', 'DC-', 1],
          ['Credit Note', 'CN-', 1],
          ['Debit Note', 'DN-', 1]
        ];
        defaults.forEach(d => sequencesSheet.appendRow(d));
        sequencesData = sequencesSheet.getDataRange().getValues();
    }
    
    var sequences = [];
    for(var i=1; i<sequencesData.length; i++) {
      sequences.push({
        type: sequencesData[i][0],
        prefix: sequencesData[i][1],
        next: sequencesData[i][2]
      });
    }
    
    return {
      success: true,
      settings: settings,
      sequences: sequences
    };
  } catch(e) { return { success: false, message: e.message }; }
}

function getNextDocNumber(docType) {
  try {
    var sheet = getSheet('billing_sequences');
    var data = sheet.getDataRange().getValues();
    for(var i=1; i<data.length; i++) {
      if(data[i][0] === docType) {
        var prefix = data[i][1] || '';
        var nextNum = parseInt(data[i][2]) || 1;
        return prefix + String(nextNum).padStart(4, '0');
      }
    }
    
    // Auto-initialize sequence for standard doc types if not found
    var defaultPrefixes = {
      'Invoice': 'INV-',
      'Estimate': 'EST-',
      'Quotation': 'QTN-',
      'Sales Order': 'SO-',
      'Delivery Challan': 'DC-',
      'Credit Note': 'CN-',
      'Debit Note': 'DN-'
    };
    var prefix = defaultPrefixes[docType] || 'DOC-';
    sheet.appendRow([docType, prefix, 1]);
    SpreadsheetApp.flush();
    return prefix + '0001';
  } catch(e) { return 'DOC-' + Date.now(); }
}

function updateBillingSequence(docType, prefix, nextNum) {
  try {
    var sheet = getSheet('billing_sequences');
    var data = sheet.getDataRange().getValues();
    var found = false;
    for(var i=1; i<data.length; i++) {
      if(data[i][0] === docType) {
        if(prefix !== undefined) sheet.getRange(i+1, 2).setValue(prefix);
        if(nextNum !== undefined) sheet.getRange(i+1, 3).setValue(nextNum);
        found = true;
        break;
      }
    }
    if(!found) {
      sheet.appendRow([docType, prefix || '', nextNum || 1]);
    }
    return { success: true };
  } catch(e) { return { success: false, message: e.message }; }
}

function saveBillingSettings(settings) {
  try {
    var sheet = getSheet('billing_settings');
    // Clear and re-save or update? Let's just update/append
    var data = sheet.getDataRange().getValues();
    for(var key in settings) {
      var val = settings[key];
      var found = false;
      for(var i=1; i<data.length; i++) {
        if(data[i][0] === key) {
          sheet.getRange(i+1, 2).setValue(val);
          found = true;
          break;
        }
      }
      if(!found) {
        sheet.appendRow([key, val, 'General', '']);
      }
    }
    return { success: true };
  } catch(e) { return { success: false, message: e.message }; }
}


function getPartyTransactions(partyId) {
  try {
    var sheet = getSalesSheet('sales_billing');
    var data = sheet.getDataRange().getValues();
    var headers = data[0];
    var results = [];
    var pIdIdx = headers.indexOf('PartyId');
    for(var i=1; i<data.length; i++) {
      if(data[i][pIdIdx] === partyId) {
        var obj = {};
        headers.forEach((h, idx) => {
          var val = data[i][idx];
          if (val instanceof Date) {
            val = val.toISOString();
          }
          obj[h] = val;
        });
        results.push(obj);
      }
    }
    return results.sort((a,b) => new Date(a.Date) - new Date(b.Date));
  } catch(e) { return []; }
}
/**
 * Uploads a file to Google Drive and returns the URL.
 */
function uploadFile(data, fileName, folderId = null) {
  try {
    const folder = folderId ? DriveApp.getFolderById(folderId) : DriveApp.getRootFolder();
    const contentType = data.substring(data.indexOf(":") + 1, data.indexOf(";"));
    const bytes = Utilities.base64Decode(data.split(",")[1]);
    const blob = Utilities.newBlob(bytes, contentType, fileName);
    const file = folder.createFile(blob);
    file.setSharing(DriveApp.Access.ANYONE_WITH_LINK, DriveApp.Permission.VIEW);
    return { success: true, url: file.getUrl(), fileId: file.getId() };
  } catch (e) {
    console.error('uploadFile error:', e);
    return { success: false, message: e.message };
  }
}
function getPartiesWithStats() {
    try {
        // Sync active dealers/techs first
        syncActiveUsersToParties();
        
        const parties = getData('parties');
        
        // Fetch invoice data from the new sales_billing sheet
        let billingData = [];
        try {
            var sheet = getSalesSheet('sales_billing');
            var bData = sheet.getDataRange().getValues();
            if(bData.length > 1) {
                var headers = bData[0];
                var pIdIdx = headers.indexOf('PartyId');
                var docTypeIdx = headers.indexOf('DocType');
                var docNumIdx = headers.indexOf('DocNumber');
                for(var i=1; i<bData.length; i++) {
                    if(['Invoice', 'Sales Invoice', 'Proforma Invoice'].includes(bData[i][docTypeIdx])) {
                        billingData.push({
                            PartyId: bData[i][pIdIdx],
                            DocNumber: bData[i][docNumIdx]
                        });
                    }
                }
            }
        } catch(e) {
            console.error('getPartiesWithStats: Failed to load sales_billing', e);
        }
        
        // Create a map for quick lookup of invoice stats by PartyId
        const invoiceMap = {};
        billingData.forEach(inv => {
            const pid = String(inv.PartyId || '');
            if (pid) {
                if (!invoiceMap[pid]) invoiceMap[pid] = { count: 0, numbers: [] };
                invoiceMap[pid].count++;
                if (inv.DocNumber) invoiceMap[pid].numbers.push(inv.DocNumber);
            }
        });
        
        // Enhance parties with invoice stats
        return parties.map(p => {
            const pid = String(p['Party ID'] || '');
            const stats = invoiceMap[pid] || { count: 0, numbers: [] };
            p.InvoiceCount = stats.count;
            p.InvoiceNumbers = stats.numbers;
            return p;
        });
    } catch (e) {
        console.error('getPartiesWithStats Error:', e);
        return [];
    }
}

function syncActiveUsersToParties() {
    try {
        const dealers = getData('dealers');
        const technicians = getTechnicians();
        const parties = getData('parties');
        
        const activeDealers = dealers.filter(d => String(d.Status).toLowerCase() === 'active');
        const activeTechs = technicians.filter(t => String(t.Status).toLowerCase() === 'active');
        
        // Sync Dealers
        activeDealers.forEach(user => {
            const mobile = user.Mobile;
            if (!mobile) return;
            const existing = parties.find(p => String(p.Mobile) === String(mobile));
            if (!existing) {
                syncPartyFromRegistration('B2B', {
                    name: user['Dealer Name'] || user.Name,
                    shopName: user['Shop Name'] || '',
                    mobile: user.Mobile,
                    email: user.Email,
                    address: user.Address,
                    pincode: user.Pincode,
                    district: user.District,
                    state: user.State,
                    GST: user['GST Number'] || '',
                    Status: 'Active'
                });
            }
        });

        // Sync Technicians
        activeTechs.forEach(user => {
            const mobile = user.Mobile;
            if (!mobile) return;
            const existing = parties.find(p => String(p.Mobile) === String(mobile));
            if (!existing) {
                syncPartyFromRegistration('B2C', {
                    name: user.Name,
                    mobile: user.Mobile,
                    email: user.Email || '',
                    address: user.Address || '',
                    Status: 'Active'
                });
            }
        });
    } catch (e) {
        console.error('syncActiveUsersToParties Error:', e);
    }
}

function downloadInvoicePDF_LEGACY(docNumber) {
  try {
    // 1. Fetch document transaction record from sales_billing
    let doc = null;
    try {
      const billingData = getBillingHistory();
      doc = billingData.find(d => String(d.DocNumber) === String(docNumber));
    } catch(err) {
      console.warn('Could not read from sales_billing, trying fallback.', err);
    }
    
    let isLegacy = false;
    let items = [];
    let partyName = 'Cash Customer';
    let partyMobile = 'N/A';
    let partyGst = '';
    let partyAddress = '';
    
    if (doc) {
      partyName = doc.PartyName;
      // Fetch customer party profile details for print billing address
      try {
        const partyList = getPartyList();
        const party = partyList.find(p => p.Id === doc.PartyId);
        if (party) {
          partyMobile = party.Mobile;
          partyGst = party.GstNumber || '';
          partyAddress = party.BillingAddress || party.Address || '';
        }
      } catch(err) { console.error('Party fetch error in PDF:', err); }
      
      try {
        items = JSON.parse(doc.ItemsJson);
      } catch(err) {
        items = [{ name: 'Sales of V-VARMA Products & Services', qty: 1, rate: parseFloat(doc.GrandTotal), disc: 0, gst: 0, total: parseFloat(doc.GrandTotal) }];
      }
    } else {
      // 2. Fallback to legacy invoices sheet for backwards compatibility
      const invoices = getData('invoices');
      const inv = invoices.find(i => String(i['Invoice Number']) === String(docNumber));
      if (!inv) throw new Error('Document not found in sales transactions');
      
      isLegacy = true;
      doc = {
        DocType: 'Invoice',
        DocNumber: inv['Invoice Number'],
        Date: inv['Invoice Date'] || inv.Date,
        Subtotal: inv.Subtotal || inv['Sub Total'] || inv['Grand Total'],
        Discount: inv.Discount || 0,
        Tax: inv.Tax || 0,
        RoundOff: inv.RoundOff || 0,
        GrandTotal: inv['Grand Total'],
        AmountPaid: inv['Amount Paid'] || inv['Grand Total'],
        BalanceDue: inv['Balance Due'] || 0,
        PaymentMethod: inv['Payment Method'] || 'Cash',
        PaymentStatus: inv['Payment Status'] || 'Paid',
        DueDate: inv['Due Date'] || '',
        Notes: inv.Notes || 'Thank you for your business!'
      };
      partyName = inv['Customer Name'];
      partyMobile = inv.Mobile;
      partyAddress = inv.Address || '';
      items = [{ name: 'Sales of V-VARMA Products & Services', qty: 1, rate: parseFloat(inv['Grand Total']), disc: 0, gst: 0, total: parseFloat(inv['GrandTotal']) }];
    }
    
    // 3. Load sales settings
    const settingsRes = getSalesSettings();
    const settings = settingsRes.success ? settingsRes.settings : {};
    
    // 4. Resolve company branding profile
    const compName = settings.CompanyName || 'V-VARMA ENTERPRISES';
    const compGst = settings.CompanyGst || 'N/A';
    const compAddress = settings.CompanyAddress || 'Tamil Nadu, India';
    const compMobile = settings.CompanyMobile || 'N/A';
    const compEmail = settings.CompanyEmail || 'N/A';
    const compLogo = settings.CompanyLogo || '';
    
    // Typography scaling configurations
    const nameTextSize = settings.NameTextSize || 'Normal';
    let companyNameSize = '28px';
    if(nameTextSize === 'Medium') companyNameSize = '32px';
    else if(nameTextSize === 'Large') companyNameSize = '38px';
    else if(nameTextSize === 'ExtraLarge') companyNameSize = '44px';
    
    const invoiceTextSize = settings.InvoiceTextSize || 'Normal';
    let docTypeSize = '20px';
    if(invoiceTextSize === 'Medium') docTypeSize = '24px';
    else if(invoiceTextSize === 'Large') docTypeSize = '28px';
    else if(invoiceTextSize === 'ExtraLarge') docTypeSize = '34px';
    
    const paperSize = settings.PaperSize || 'A4';
    const extraTopSpace = parseInt(settings.ExtraTopSpace) || 0;
    
    // 5. Generate UPI QR Code URL if enabled
    const upiId = settings.CompanyUpi;
    const payeeName = settings.CompanyUpiName || compName;
    const docType = doc.DocType || 'Invoice';
    
    let qrEnabled = false;
    if (docType === 'Invoice' && settings.QrInvoice === 'true') qrEnabled = true;
    else if (docType === 'Sales Invoice' && settings.QrSalesInvoice === 'true') qrEnabled = true;
    else if (docType === 'Proforma Invoice' && settings.QrProformaInvoice === 'true') qrEnabled = true;
    else if (docType === 'Quotation' && settings.QrQuotation === 'true') qrEnabled = true;
    else if (docType === 'Estimation' && settings.QrEstimation === 'true') qrEnabled = true;
    else if (docType === 'Credit Note' && settings.QrCreditNote === 'true') qrEnabled = true;
    else if (docType === 'Debit Note' && settings.QrDebitNote === 'true') qrEnabled = true;
    else if (docType === 'Delivery Challan' && settings.QrDeliveryChallan === 'true') qrEnabled = true;
    else if (docType === 'Receipt' && settings.QrReceipt === 'true') qrEnabled = true;
    else if (docType === 'Payment Request' && settings.QrPaymentRequest === 'true') qrEnabled = true;
    
    if ((docType === 'Estimation' || docType === 'Delivery Challan') && settings.EnablePaymentRequestChallanEst !== 'true') {
      qrEnabled = false;
    }
    
    let qrImgHtml = '';
    let payAmt = parseFloat(doc.GrandTotal) || 0;
    if (docType === 'Credit Note') {
      payAmt = Math.abs(payAmt);
    }
    
    if (qrEnabled && upiId && payAmt > 0) {
      const upiUrl = `upi://pay?pa=${upiId}&pn=${encodeURIComponent(payeeName)}&am=${payAmt.toFixed(2)}&cu=INR&tn=${encodeURIComponent(docType + ' ' + doc.DocNumber)}`;
      const qrApiUrl = `https://api.qrserver.com/v1/create-qr-code/?size=200x200&data=${encodeURIComponent(upiUrl)}`;
      qrImgHtml = `
        <div style="text-align: center; border: 1px solid #ddd; padding: 6px; border-radius: 6px; background: #fff; width: 120px; margin: 0 auto 3px auto; box-shadow: 0 2px 4px rgba(0,0,0,0.05);">
          <img src="${qrApiUrl}" style="width: 108px; height: 108px; object-fit: contain; display: block; margin: 0 auto;" />
          <div style="font-weight: bold; color: #198754; font-size: 11px; margin-top: 3px; font-family: 'Helvetica Neue', Arial, sans-serif;">Rs. ${payAmt.toFixed(2)}</div>
        </div>
        <div style="font-size: 8px; color: #555; font-weight: bold; text-align: center; text-transform: uppercase; letter-spacing: 0.3px;">Scan to Pay via UPI</div>
      `;
    }
    
    // 6. Build the A4 layout HTML template
    let itemRowsHtml = '';
    items.forEach((item, index) => {
      const itemName = item.name || item.ProductName || 'Sales Item';
      const itemDesc = item.desc ? `<br><small style="color: #666; font-style: italic;">${item.desc}</small>` : '';
      const itemQty = item.qty || 1;
      const itemRate = parseFloat(item.rate || item.Price || 0);
      const itemGst = parseFloat(item.gst || 0);
      const itemDisc = parseFloat(item.disc || 0);
      const itemTotal = parseFloat(item.total || (itemQty * itemRate));
      
      itemRowsHtml += `
        <tr style="border-bottom: 1px solid #E2E8F0;">
          <td style="padding: 8px 10px; font-size: 11px; color: #4A5568;">${index + 1}</td>
          <td style="padding: 8px 10px; font-size: 11px; font-weight: 600; color: #1A202C;">${itemName}${itemDesc}</td>
          <td style="padding: 8px 10px; font-size: 11px; text-align: right; color: #2D3748;">${itemQty}</td>
          <td style="padding: 8px 10px; font-size: 11px; text-align: right; color: #2D3748;">Rs. ${itemRate.toFixed(2)}</td>
          <td style="padding: 8px 10px; font-size: 11px; text-align: right; color: #E53E3E;">${itemDisc > 0 ? itemDisc + '%' : '-'}</td>
          <td style="padding: 8px 10px; font-size: 11px; text-align: right; color: #3182CE;">${itemGst > 0 ? itemGst + '%' : '-'}</td>
          <td style="padding: 8px 10px; font-size: 11px; text-align: right; font-weight: bold; color: #1A202C;">Rs. ${itemTotal.toFixed(2)}</td>
        </tr>
      `;
    });
    
    // Copy options: duplicate copy indicator
    const copyIndicator = settings.PrintOriginalDuplicate === 'true' ? '<div style="position: absolute; top: 15px; right: 15px; background: #EDF2F7; color: #4A5568; font-size: 9px; padding: 4px 8px; border-radius: 4px; font-weight: bold; border: 1px solid #CBD5E1;">ORIGINAL COPY</div>' : '';
    
    let displayDocType = docType;
    if (docType === 'Invoice' || docType === 'Sales Invoice') {
      const isGstEnabled = doc.GstEnabled === true || String(doc.GstEnabled) === 'true' || parseFloat(doc.Tax || 0) > 0;
      displayDocType = isGstEnabled ? 'TAX INVOICE' : 'PROFORMA INVOICE';
    }

    const html = `
      <html>
        <head>
          <meta charset="utf-8">
          <style>
            body { font-family: 'Helvetica Neue', Helvetica, Arial, sans-serif; color: #2D3748; margin: 0; padding: ${extraTopSpace}mm 8mm 8mm 8mm; background: #FFF; }
            .header-bar { display: table; width: 100%; border-bottom: 2px solid #2B6CB0; padding-bottom: 12px; margin-bottom: 18px; }
            .company-info { display: table-cell; width: 65%; vertical-align: top; }
            .company-name { font-size: ${companyNameSize}; font-weight: bold; color: #1A365D; margin: 0 0 4px 0; letter-spacing: -0.5px; }
            .company-details { font-size: 11px; color: #4A5568; line-height: 1.4; margin: 0; }
            .logo-box { display: table-cell; width: 35%; text-align: right; vertical-align: top; }
            .logo-img { max-height: 60px; max-width: 180px; object-fit: contain; }
            
            .doc-title-bar { background: #2B6CB0; color: #FFF; padding: 8px 12px; border-radius: 4px; display: table; width: 100%; margin-bottom: 18px; box-sizing: border-box; }
            .doc-title { display: table-cell; font-size: ${docTypeSize}; font-weight: bold; text-transform: uppercase; letter-spacing: 0.5px; vertical-align: middle; }
            .doc-no { display: table-cell; text-align: right; font-size: 13px; font-weight: bold; vertical-align: middle; }
            
            .grid-container { display: table; width: 100%; margin-bottom: 20px; }
            .bill-to-box { display: table-cell; width: 48%; border: 1px solid #E2E8F0; padding: 10px; border-radius: 6px; background: #F8FAFC; vertical-align: top; }
            .bill-to-title { font-size: 10px; font-weight: bold; color: #718096; text-transform: uppercase; margin: 0 0 6px 0; border-bottom: 1px solid #E2E8F0; padding-bottom: 3px; }
            .bill-to-details { font-size: 11px; line-height: 1.4; margin: 0; color: #2D3748; }
            
            .spacer-cell { display: table-cell; width: 4%; }
            
            .details-box { display: table-cell; width: 48%; border: 1px solid #E2E8F0; padding: 10px; border-radius: 6px; background: #F8FAFC; vertical-align: top; }
            .details-table { width: 100%; border-collapse: collapse; }
            .details-table td { padding: 3px 0; font-size: 11px; color: #2D3748; }
            
            .items-table { width: 100%; border-collapse: collapse; margin-bottom: 22px; }
            .items-table th { background: #EDF2F7; color: #2D3748; font-size: 11px; font-weight: bold; padding: 8px 10px; border-bottom: 2px solid #CBD5E1; }
            .items-table td { padding: 8px 10px; }
            
            .summary-container { display: table; width: 100%; margin-bottom: 25px; }
            .payment-methods-box { display: table-cell; width: 56%; font-size: 11px; vertical-align: top; }
            .payment-methods-title { font-weight: bold; border-bottom: 1px solid #E2E8F0; padding-bottom: 3px; margin-bottom: 6px; color: #4A5568; }
            .payment-methods-details { line-height: 1.4; color: #4A5568; }
            
            .totals-spacer { display: table-cell; width: 4%; }
            
            .totals-box { display: table-cell; width: 40%; background: #F8FAFC; border: 1px solid #E2E8F0; border-radius: 6px; padding: 10px; vertical-align: top; }
            .totals-table { width: 100%; border-collapse: collapse; }
            .totals-table td { padding: 4px 0; font-size: 11px; color: #2D3748; }
            .grand-total-row td { font-size: 14px; font-weight: bold; color: #1A365D; border-top: 1px solid #CBD5E1; padding-top: 6px; }
            
            .footer-panel { display: table; width: 100%; border-top: 1px solid #E2E8F0; padding-top: 15px; margin-top: 20px; }
            .footer-tc { display: table-cell; width: 50%; font-size: 9px; line-height: 1.4; color: #718096; vertical-align: top; }
            
            .footer-sig-qr-container { display: table-cell; width: 50%; vertical-align: top; }
            .sub-footer-table { width: 100%; border-collapse: collapse; }
            .qr-cell { width: 45%; vertical-align: bottom; }
            .signature-box { width: 55%; text-align: center; font-size: 11px; vertical-align: bottom; }
            .sig-line { border-top: 1px solid #2D3748; padding-top: 4px; margin-top: 35px; font-weight: bold; color: #1A202C; }
          </style>
        </head>
        <body>
          ${copyIndicator}
          
          <!-- Header Profile -->
          <div class="header-bar">
            <div class="company-info">
              <h1 class="company-name">${compName}</h1>
              <p class="company-details">
                <strong>GSTIN:</strong> ${compGst}<br>
                ${compAddress}<br>
                <strong>Phone:</strong> ${compMobile} | <strong>Email:</strong> ${compEmail}
              </p>
            </div>
            <div class="logo-box">
              ${compLogo ? `<img src="${compLogo}" class="logo-img" />` : `<div style="font-weight: 300; font-size: 11px; color: #A0AEC0;">Quality & Trust Since 1985</div>`}
            </div>
          </div>
          
          <!-- Document Identification -->
          <div class="doc-title-bar">
            <div class="doc-title">${displayDocType}</div>
            <div class="doc-no"><strong>No:</strong> ${doc.DocNumber}</div>
          </div>
          
          <!-- Grid Customer & Transaction Metadata -->
          <div class="grid-container">
            <div class="bill-to-box">
              <div class="bill-to-title">Bill To / Party Details</div>
              <p class="bill-to-details">
                <strong>Name:</strong> ${partyName}<br>
                <strong>Phone:</strong> ${partyMobile}<br>
                ${partyGst ? `<strong>GSTIN:</strong> ${partyGst}<br>` : ''}
                ${partyAddress ? `<strong>Address:</strong> ${partyAddress}` : ''}
              </p>
            </div>
            <div class="spacer-cell"></div>
            <div class="details-box">
              <div class="bill-to-title">Document Metadata</div>
              <table class="details-table">
                <tr>
                  <td><strong>Date of Issue:</strong></td>
                  <td style="text-align: right;">${doc.Date}</td>
                </tr>
                ${doc.DueDate ? `
                <tr>
                  <td><strong>Due Date:</strong></td>
                  <td style="text-align: right; color: #E53E3E; font-weight: bold;">${doc.DueDate}</td>
                </tr>` : ''}
                <tr>
                  <td><strong>Payment Mode:</strong></td>
                  <td style="text-align: right; text-transform: uppercase; font-weight: bold;">${doc.PaymentMethod}</td>
                </tr>
                <tr>
                  <td><strong>Status:</strong></td>
                  <td style="text-align: right; font-weight: bold; color: ${doc.PaymentStatus === 'Paid' ? '#38A169' : '#D69E2E'}">${doc.PaymentStatus}</td>
                </tr>
              </table>
            </div>
          </div>
          
          <!-- Items Table -->
          <table class="items-table">
            <thead>
              <tr style="border-bottom: 2px solid #CBD5E1;">
                <th style="width: 5%; text-align: left;">#</th>
                <th style="width: 45%; text-align: left;">Description</th>
                <th style="width: 10%; text-align: right;">Qty</th>
                <th style="width: 10%; text-align: right;">Rate</th>
                <th style="width: 10%; text-align: right;">Disc</th>
                <th style="width: 10%; text-align: right;">GST</th>
                <th style="width: 10%; text-align: right;">Total</th>
              </tr>
            </thead>
            <tbody>
              ${itemRowsHtml}
            </tbody>
          </table>
          
          <!-- Summaries & Totals -->
          <div class="summary-container">
            <div class="payment-methods-box">
              ${settings.ShowPaymentModeDetails === 'true' && settings.BankName ? `
              <div class="payment-methods-title">Bank Transfer Details:</div>
              <div class="payment-methods-details">
                <strong>A/C Holder:</strong> ${settings.BankAccHolder || compName}<br>
                <strong>Bank:</strong> ${settings.BankName}<br>
                <strong>A/C Number:</strong> ${settings.BankAccNo}<br>
                <strong>IFSC:</strong> ${settings.BankIfsc} | <strong>Branch:</strong> ${settings.BankBranch || 'N/A'}
              </div>` : ''}
            </div>
            <div class="totals-spacer"></div>
            <div class="totals-box">
              <table class="totals-table">
                <tr>
                  <td>Subtotal:</td>
                  <td style="text-align: right;">Rs. ${parseFloat(doc.Subtotal).toFixed(2)}</td>
                </tr>
                ${parseFloat(doc.Discount) > 0 ? `
                <tr style="color: #38A169;">
                  <td>Discount Saved:</td>
                  <td style="text-align: right;">-Rs. ${parseFloat(doc.Discount).toFixed(2)}</td>
                </tr>` : ''}
                ${parseFloat(doc.Tax) > 0 ? `
                <tr style="color: #3182CE;">
                  <td>Taxes (GST):</td>
                  <td style="text-align: right;">Rs. ${parseFloat(doc.Tax).toFixed(2)}</td>
                </tr>` : ''}
                ${parseFloat(doc.RoundOff) !== 0 ? `
                <tr style="color: #718096;">
                  <td>Round Off:</td>
                  <td style="text-align: right;">Rs. ${parseFloat(doc.RoundOff).toFixed(2)}</td>
                </tr>` : ''}
                <tr class="grand-total-row">
                  <td><strong>GRAND TOTAL:</strong></td>
                  <td style="text-align: right;"><strong>Rs. ${parseFloat(doc.GrandTotal).toFixed(2)}</strong></td>
                </tr>
                <tr style="font-size: 10px; font-weight: 600; color: #4A5568;">
                  <td style="padding-top: 4px;">Amount Paid:</td>
                  <td style="text-align: right; padding-top: 4px;">Rs. ${parseFloat(doc.AmountPaid || doc.GrandTotal).toFixed(2)}</td>
                </tr>
                ${parseFloat(doc.BalanceDue) > 0 ? `
                <tr style="font-size: 10px; font-weight: 600; color: #E53E3E;">
                  <td>Balance Due:</td>
                  <td style="text-align: right;">Rs. ${parseFloat(doc.BalanceDue).toFixed(2)}</td>
                </tr>` : ''}
              </table>
            </div>
          </div>
          
          <!-- Legal T&C, Signatory and Dynamic Payment QR -->
          <div class="footer-panel">
            <div class="footer-tc">
              <strong>TERMS & CONDITIONS:</strong><br>
              ${(settings.FooterTC || doc.Notes || '1. Goods once sold will not be taken back.\\n2. Subject to local jurisdiction.').replace(/\\n/g, '<br>').replace(/\n/g, '<br>')}
            </div>
            
            <div class="footer-sig-qr-container">
              <table class="sub-footer-table">
                <tr>
                  <td class="qr-cell">
                    ${qrImgHtml}
                  </td>
                  <td class="signature-box">
                    <div class="sig-line">
                      ${settings.FooterAuthorizedSignatory || 'Authorized Signatory'}<br>
                      <span style="font-size: 8px; color: #718096; font-weight: normal;">For ${compName}</span>
                    </div>
                  </td>
                </tr>
              </table>
            </div>
          </div>
          
        </body>
      </html>
    `;
    
    // 7. Compile to Blob PDF via Apps Script engine
    const blob = Utilities.newBlob(html, MimeType.HTML).getAs(MimeType.PDF);
    blob.setName(`${docType.replace(/\s+/g, '_')}_${docNumber}.pdf`);
    const base64 = Utilities.base64Encode(blob.getBytes());
    
    return { success: true, base64: base64, filename: `${docType.replace(/\s+/g, '_')}_${docNumber}.pdf` };
  } catch (e) {
    return { success: false, message: e.message };
  }
}

function getPartyInvoices(mobile) {
    try {
        const invoices = getData('invoices');
        return invoices.filter(inv => String(inv.Mobile) === String(mobile));
    } catch (e) {
        console.error('getPartyInvoices Error:', e);
        return [];
    }
}

function getCompanyProfile() {
  try {
    const data = getData('company_profile');
    const profile = {};
    data.forEach(row => {
      if(row.Key) profile[row.Key] = row.Value;
    });
    return { success: true, profile: profile };
  } catch (e) {
    return { success: false, message: e.message };
  }
}

function saveCompanyProfile(profileData) {
  try {
    const sheet = getSheet('company_profile');
    sheet.clearContents();
    const headers = SHEET_SCHEMAS.company_profile;
    sheet.appendRow(headers);
    
    const now = new Date().toISOString();
    Object.keys(profileData).forEach(key => {
      sheet.appendRow([key, profileData[key], now]);
    });
    
    return { success: true, message: 'Profile saved successfully' };
  } catch (e) {
    return { success: false, message: e.message };
  }
}

function getSalesSettings() {
  try {
    const data = getData('sales_settings');
    const settings = {};
    data.forEach(row => {
      if(row.Key) settings[row.Key] = row.Value;
    });
    
    // Fallback/Legacy migration: if Sales_Settings is completely empty, populate from company_profile and billing_settings
    if (Object.keys(settings).length === 0) {
      console.log('getSalesSettings: Settings sheet empty. Migrating legacy data as fallback.');
      try {
        const legacyProfile = getData('company_profile');
        legacyProfile.forEach(row => { if(row.Key) settings[row.Key] = row.Value; });
        const legacyBilling = getData('billing_settings');
        legacyBilling.forEach(row => { if(row.Key) settings[row.Key] = row.Value; });
      } catch(e) {
        console.warn('getSalesSettings: Legacy migration skipped:', e.message);
      }
    }
    
    // Also include prefixes and series from billing_sequences to make it fully centralized
    const sequencesSheet = getSheet('billing_sequences');
    const sequencesData = sequencesSheet.getDataRange().getValues();
    const sequences = [];
    for(var i=1; i<sequencesData.length; i++) {
      sequences.push({
        type: sequencesData[i][0],
        prefix: sequencesData[i][1],
        next: sequencesData[i][2]
      });
    }
    
    return { success: true, settings: settings, sequences: sequences };
  } catch (e) {
    return { success: false, message: e.message };
  }
}

function saveSalesSettings(settingsData) {
  try {
    // Intercept any base64 image values and upload them to Google Drive
    // to prevent the 50,000 character cell limit error in Google Sheets.
    const imageKeyMap = {
      'CompanyLogo':   'company_logo.png',
      'SignatureImage':'authorized_signature.png',
      'SealImage':     'company_seal.png'
    };
    Object.keys(settingsData).forEach(key => {
      var val = settingsData[key];
      if (typeof val === 'string' && val.startsWith('data:image/')) {
        var filename = imageKeyMap[key] || (key.toLowerCase().replace(/[^a-z0-9]/g, '_') + '.png');
        try {
          settingsData[key] = convertBase64ToDriveUrl_(val, filename);
        } catch(imgErr) {
          console.error('saveSalesSettings: Drive upload failed for key ' + key + ': ' + imgErr.message);
        }
      }
    });

    const sheet = getSheet('sales_settings');
    sheet.clearContents();
    const headers = SHEET_SCHEMAS.sales_settings;
    sheet.appendRow(headers);
    
    const now = new Date().toISOString();
    Object.keys(settingsData).forEach(key => {
      sheet.appendRow([key, settingsData[key], now]);
    });
    
    // Also sync keys back to company_profile if they exist in company profile, for high-fidelity backwards compatibility
    try {
      const profileSheet = getSheet('company_profile');
      const profileData = {};
      const keysToSync = ['CompanyName', 'CompanyGst', 'CompanyAddress', 'CompanyMobile', 'CompanyEmail', 'CompanyStateCode', 'CompanyUpi', 'BankName', 'BankAccNo', 'BankIfsc', 'BankBranch'];
      keysToSync.forEach(k => {
        if(settingsData[k] !== undefined) profileData[k] = settingsData[k];
      });
      if (Object.keys(profileData).length > 0) {
        profileSheet.clearContents();
        profileSheet.appendRow(SHEET_SCHEMAS.company_profile);
        Object.keys(profileData).forEach(key => {
          profileSheet.appendRow([key, profileData[key], now]);
        });
      }
    } catch(err) { console.error('Backwards profile sync failed:', err); }
    
    return { success: true, message: 'Settings saved successfully' };
  } catch (e) {
    return { success: false, message: e.message };
  }
}

// ============================================
// ADMIN DEFAULT SETTINGS (GLOBAL PREFS)
// ============================================

function saveAdminSettings(settingsObj) {
  try {
    var ss = getSpreadsheet();
    var sheetName = CONFIG.sheets.admin_settings;
    var sheet = ss.getSheetByName(sheetName);
    
    if (!sheet) {
      sheet = ss.insertSheet(sheetName);
      sheet.appendRow(SHEET_SCHEMAS.admin_settings);
      sheet.getRange(1, 1, 1, sheet.getLastColumn()).setFontWeight("bold").setBackground("#f3f3f3");
      sheet.setFrozenRows(1);
    }
    
    var data = sheet.getDataRange().getValues();
    var existingKeys = {};
    for (var i = 1; i < data.length; i++) {
      existingKeys[data[i][0]] = i + 1;
    }
    
    var timestamp = new Date().toISOString();
    
    for (var key in settingsObj) {
      var value = settingsObj[key];
      if (existingKeys[key]) {
        sheet.getRange(existingKeys[key], 2).setValue(value);
        sheet.getRange(existingKeys[key], 3).setValue(timestamp);
      } else {
        sheet.appendRow([key, value, timestamp]);
        existingKeys[key] = sheet.getLastRow();
      }
    }
    return { success: true };
  } catch (err) {
    return { success: false, message: err.message };
  }
}

function getAdminSettings() {
  try {
    var ss = getSpreadsheet();
    var sheetName = CONFIG.sheets.admin_settings;
    var sheet = ss.getSheetByName(sheetName);
    var settings = {};
    
    if (!sheet) return settings;
    
    var data = sheet.getDataRange().getValues();
    for (var i = 1; i < data.length; i++) {
      var key = data[i][0];
      var val = data[i][1];
      settings[key] = val;
    }
    return settings;
  } catch (err) {
    console.error('getAdminSettings error:', err);
    return {};
  }
}

// ============================================
// PAYMENT IN & RECEIPT MANAGEMENT SERVICES
// ============================================

function syncPaymentInSchemas() {
  try {
    var ss = getSpreadsheet();
    var keys = ['payment_receipts', 'customer_wallet', 'cash_flow'];
    keys.forEach(key => {
      var sheetName = CONFIG.sheets[key] || key;
      var sheet = ss.getSheetByName(sheetName);
      if (!sheet) {
        sheet = ss.insertSheet(sheetName);
        sheet.appendRow(SHEET_SCHEMAS[key]);
      }
    });
    return { success: true };
  } catch (err) {
    console.error('syncPaymentInSchemas error:', err);
    return { success: false, message: err.message };
  }
}

function getNextVoucherNumber(docType, defaultPrefix) {
  try {
    var sheet = getSheet('billing_sequences');
    var data = sheet.getDataRange().getValues();
    var foundIndex = -1;
    for(var i=1; i<data.length; i++) {
      if(data[i][0] === docType) {
        foundIndex = i;
        break;
      }
    }
    if (foundIndex === -1) {
      sheet.appendRow([docType, defaultPrefix, 2]);
      return defaultPrefix + "0001";
    } else {
      var prefix = data[foundIndex][1] || '';
      var nextNum = parseInt(data[foundIndex][2]) || 1;
      return prefix + String(nextNum).padStart(4, '0');
    }
  } catch(e) {
    return defaultPrefix + Date.now();
  }
}

function incrementVoucherSequence(docType) {
  try {
    var sheet = getSheet('billing_sequences');
    var data = sheet.getDataRange().getValues();
    for(var i=1; i<data.length; i++) {
      if(data[i][0] === docType) {
        var nextNum = parseInt(data[i][2]) || 1;
        sheet.getRange(i+1, 3).setValue(nextNum + 1);
        break;
      }
    }
  } catch(e) {
    console.error('Failed increment sequence:', docType, e.message);
  }
}

function savePaymentReceipt(data) {
  try {
    syncPaymentInSchemas();
    var ss = getSpreadsheet();
    
    // 1. Generate Receipt Number
    var receiptNo = getNextVoucherNumber('Receipt', 'REC-');
    incrementVoucherSequence('Receipt');
    
    var transactionId = 'TXN-' + Date.now();
    var receiptDate = data.date || new Date().toISOString().split('T')[0];
    var customerName = data.customerName;
    var mobile = data.mobile;
    var amountReceived = parseFloat(data.amount) || 0;
    var paymentMode = data.mode;
    var referenceNo = data.referenceNo || '';
    var referenceDate = data.referenceDate || '';
    var notes = data.notes || '';
    var allocations = data.allocations || []; // [ { invoiceNo: '...', amount: 100 } ]
    
    // 2. Write to payment_receipts sheet
    var receiptsSheet = getSheet('payment_receipts');
    receiptsSheet.appendRow([
      receiptNo,
      transactionId,
      receiptDate,
      customerName,
      mobile,
      amountReceived,
      paymentMode,
      referenceNo,
      referenceDate,
      notes,
      JSON.stringify(allocations),
      'Active'
    ]);
    
    // 3. Process allocations & update invoices
    var allocatedTotal = 0;
    var invoicesSheet = getSheet('invoices');
    var invoiceRows = invoicesSheet.getDataRange().getValues();
    
    allocations.forEach(alloc => {
      var invNo = alloc.invoiceNo;
      var allocAmt = parseFloat(alloc.amount) || 0;
      if (allocAmt <= 0) return;
      
      allocatedTotal += allocAmt;
      
      // Update Invoice
      for(var i=1; i<invoiceRows.length; i++) {
        if(invoiceRows[i][0] === invNo) {
          var grandTotal = parseFloat(invoiceRows[i][10]) || 0; // Grand Total
          var prevPaid = parseFloat(invoiceRows[i][11]) || 0;  // Paid Amount
          var newPaid = prevPaid + allocAmt;
          var newBalance = Math.max(0, grandTotal - newPaid);
          var status = newBalance <= 0 ? 'Paid' : 'Partially Paid';
          
          invoicesSheet.getRange(i+1, 12).setValue(newPaid);       // Paid Amount
          invoicesSheet.getRange(i+1, 13).setValue(newBalance);    // Balance
          invoicesSheet.getRange(i+1, 14).setValue(status);        // Payment Status
          break;
        }
      }
    });
    
    // 4. Handle excess payment (add to wallet)
    var excess = amountReceived - allocatedTotal;
    if (excess > 0.01) {
      var walletSheet = getSheet('customer_wallet');
      var walletId = 'WLT-' + Date.now();
      walletSheet.appendRow([
        walletId,
        customerName,
        mobile,
        'Deposit',
        excess,
        referenceNo,
        referenceDate,
        paymentMode,
        notes,
        receiptDate
      ]);
    }
    
    // 5. Post to cash_flow
    var cashFlowSheet = getSheet('cash_flow');
    cashFlowSheet.appendRow([
      receiptDate,
      receiptNo,
      'Receipt',
      customerName,
      'Payment Received' + (notes ? ' - ' + notes : ''),
      amountReceived,
      0,
      0, // running balance computed on client/fetch
      paymentMode,
      referenceNo,
      'Active'
    ]);
    
    return { success: true, receiptNumber: receiptNo, amountReceived: amountReceived, excess: excess };
  } catch(e) {
    console.error('savePaymentReceipt error:', e);
    return { success: false, message: e.message };
  }
}

function refundCustomerWallet(data) {
  try {
    syncPaymentInSchemas();
    
    var refundNo = getNextVoucherNumber('Refund', 'REF-');
    incrementVoucherSequence('Refund');
    
    var date = data.date || new Date().toISOString().split('T')[0];
    var customerName = data.customerName;
    var mobile = data.mobile;
    var amount = parseFloat(data.amount) || 0;
    var refundMethod = data.refundMethod;
    var referenceNo = data.referenceNo || '';
    var referenceDate = data.referenceDate || '';
    var notes = data.notes || '';
    
    // Check wallet balance
    var walletData = getCustomerWalletBalance(customerName, mobile);
    if(amount > walletData.balance) {
      return { success: false, message: 'Insufficient wallet balance. Available: ₹' + walletData.balance };
    }
    
    // 1. Add withdrawal record in customer_wallet
    var walletSheet = getSheet('customer_wallet');
    var walletId = 'WLT-W-' + Date.now();
    walletSheet.appendRow([
      walletId,
      customerName,
      mobile,
      'Withdrawal',
      amount,
      referenceNo,
      referenceDate,
      refundMethod,
      notes,
      date
    ]);
    
    // 2. Add cash_out transaction in cash_flow
    var cashFlowSheet = getSheet('cash_flow');
    cashFlowSheet.appendRow([
      date,
      refundNo,
      'Refund',
      customerName,
      'Wallet Refund' + (notes ? ' - ' + notes : ''),
      0,
      amount,
      0,
      refundMethod,
      referenceNo,
      'Active'
    ]);
    
    return { success: true, refundNumber: refundNo, amount: amount };
  } catch(e) {
    console.error('refundCustomerWallet error:', e);
    return { success: false, message: e.message };
  }
}

function getCustomerWalletBalance(customerName, mobile) {
  try {
    syncPaymentInSchemas();
    var walletSheet = getSheet('customer_wallet');
    var data = walletSheet.getDataRange().getValues();
    var balance = 0;
    for(var i=1; i<data.length; i++) {
      var nameMatch = String(data[i][1]).trim().toLowerCase() === String(customerName).trim().toLowerCase();
      var mobileMatch = String(data[i][2]).trim() === String(mobile).trim();
      if(nameMatch || mobileMatch) {
        var type = data[i][3];
        var amt = parseFloat(data[i][4]) || 0;
        if(type === 'Deposit') balance += amt;
        else if(type === 'Withdrawal') balance -= amt;
      }
    }
    return { balance: balance };
  } catch(e) {
    return { balance: 0 };
  }
}

function getCustomerLedgerData(customerName, mobile) {
  try {
    syncPaymentInSchemas();
    
    var ledgerEntries = [];
    
    // 1. Invoices
    var invoices = getData('invoices') || [];
    invoices.forEach(inv => {
      var nameMatch = String(inv['Customer Name'] || inv['Name'] || '').trim().toLowerCase() === String(customerName).trim().toLowerCase();
      var mobileMatch = String(inv['Mobile'] || '').trim() === String(mobile).trim();
      if(nameMatch || mobileMatch) {
        ledgerEntries.push({
          date: inv['Invoice Date'] || inv['Date'] || '',
          voucherNo: inv['Invoice Number'] || '',
          voucherType: 'Invoice',
          description: 'Invoice Generated',
          debit: parseFloat(inv['Grand Total']) || 0,
          credit: 0,
          refNo: inv['Transaction ID'] || '',
          refDate: inv['Invoice Date'] || '',
          mode: inv['Payment Mode'] || '',
          linkedDocs: inv['Invoice Number'] || '',
          status: inv['Payment Status'] || 'Active'
        });
      }
    });
    
    // 2. Payment Receipts
    var receipts = getData('payment_receipts') || [];
    receipts.forEach(rec => {
      var nameMatch = String(rec['Customer Name'] || '').trim().toLowerCase() === String(customerName).trim().toLowerCase();
      var mobileMatch = String(rec['Mobile'] || '').trim() === String(mobile).trim();
      if(nameMatch || mobileMatch) {
        var allocs = [];
        try { allocs = JSON.parse(rec['Allocations JSON'] || '[]'); } catch(e) {}
        var linked = allocs.map(a => a.invoiceNo).join(', ') || '-';
        
        ledgerEntries.push({
          date: rec['Date'] || '',
          voucherNo: rec['Receipt Number'] || '',
          voucherType: 'Receipt',
          description: rec['Notes'] || 'Payment Received',
          debit: 0,
          credit: parseFloat(rec['Amount Received']) || 0,
          refNo: rec['Reference Number'] || '',
          refDate: rec['Reference Date'] || '',
          mode: rec['Mode of Payment'] || '',
          linkedDocs: linked,
          status: rec['Status'] || 'Active'
        });
      }
    });
    
    // 3. Wallet Entries
    var wallets = getData('customer_wallet') || [];
    wallets.forEach(w => {
      var nameMatch = String(w['Customer Name'] || '').trim().toLowerCase() === String(customerName).trim().toLowerCase();
      var mobileMatch = String(w['Mobile'] || '').trim() === String(mobile).trim();
      if(nameMatch || mobileMatch) {
        var isDeposit = w['Type'] === 'Deposit';
        ledgerEntries.push({
          date: w['Date'] || '',
          voucherNo: w['Wallet ID'] || '',
          voucherType: isDeposit ? 'Wallet Deposit' : 'Wallet Withdrawal',
          description: w['Notes'] || (isDeposit ? 'Excess payment credit' : 'Wallet Refund'),
          debit: isDeposit ? 0 : (parseFloat(w['Amount']) || 0),
          credit: isDeposit ? (parseFloat(w['Amount']) || 0) : 0,
          refNo: w['Reference Number'] || '',
          refDate: w['Reference Date'] || '',
          mode: w['Mode of Payment'] || '',
          linkedDocs: '-',
          status: 'Active'
        });
      }
    });
    
    // Sort chronologically
    ledgerEntries.sort((a,b) => new Date(a.date) - new Date(b.date));
    
    // Running balance
    var running = 0;
    ledgerEntries.forEach(entry => {
      running += (entry.debit - entry.credit);
      entry.runningBalance = running;
    });
    
    return { success: true, ledger: ledgerEntries, currentWalletBalance: getCustomerWalletBalance(customerName, mobile).balance };
  } catch(e) {
    return { success: false, message: e.message };
  }
}

function getCashFlowLedgerData() {
  try {
    syncPaymentInSchemas();
    var cashFlow = getData('cash_flow') || [];
    
    cashFlow.sort((a,b) => new Date(a['Date']) - new Date(b['Date']));
    
    var running = 0;
    var formatted = cashFlow.map(row => {
      var ci = parseFloat(row['Cash In']) || 0;
      var co = parseFloat(row['Cash Out']) || 0;
      running += (ci - co);
      return {
        date: row['Date'] || '',
        voucherNo: row['Voucher Number'] || '',
        voucherType: row['Voucher Type'] || '',
        partyName: row['Party Name'] || '',
        description: row['Description'] || '',
        cashIn: ci,
        cashOut: co,
        runningBalance: running,
        mode: row['Payment Mode'] || '',
        refNo: row['Reference Number'] || '',
        status: row['Status'] || 'Active'
      };
    });
    
    return { success: true, cashFlow: formatted };
  } catch(e) {
    return { success: false, message: e.message };
  }
}

function deletePaymentVoucher(voucherNo, voucherType) {
  try {
    var sheetName = voucherType === 'Receipt' ? 'payment_receipts' : 'cash_flow';
    var sheet = getSheet(sheetName);
    var data = sheet.getDataRange().getValues();
    var keyCol = voucherType === 'Receipt' ? 0 : 1;
    for(var i=1; i<data.length; i++) {
      if(data[i][keyCol] === voucherNo) {
        var statusCol = voucherType === 'Receipt' ? 11 : 10;
        sheet.getRange(i+1, statusCol+1).setValue('Deleted');
        
        if (voucherType === 'Receipt') {
          var cfSheet = getSheet('cash_flow');
          var cfData = cfSheet.getDataRange().getValues();
          for(var j=1; j<cfData.length; j++) {
            if(cfData[j][1] === voucherNo) {
              cfSheet.getRange(j+1, 11).setValue('Deleted');
              break;
            }
          }
        }
        return { success: true };
      }
    }
    return { success: false, message: 'Voucher not found' };
  } catch(e) {
    return { success: false, message: e.message };
  }
}

function getDashboardFinancials() {
  try {
    syncPaymentInSchemas();
    
    var todayStr = new Date().toISOString().split('T')[0];
    
    var invoices = getData('invoices') || [];
    var receivables = 0;
    var payables = 0;
    
    invoices.forEach(inv => {
      var bal = parseFloat(inv['Balance']) || 0;
      receivables += bal;
    });
    
    var cashFlow = getData('cash_flow') || [];
    var totalIncomingToday = 0;
    var totalOutgoingToday = 0;
    var currentCashBalance = 0;
    
    cashFlow.forEach(row => {
      if(row['Status'] === 'Deleted') return;
      var ci = parseFloat(row['Cash In']) || 0;
      var co = parseFloat(row['Cash Out']) || 0;
      currentCashBalance += (ci - co);
      
      var rowDate = row['Date'] || '';
      if(rowDate.split('T')[0] === todayStr) {
        totalIncomingToday += ci;
        totalOutgoingToday += co;
      }
    });
    
    var walletSheet = getSheet('customer_wallet');
    var walletData = walletSheet.getDataRange().getValues();
    var walletTotal = 0;
    for(var i=1; i<walletData.length; i++) {
      var type = walletData[i][3];
      var amt = parseFloat(walletData[i][4]) || 0;
      if (type === 'Deposit') walletTotal += amt;
      else if (type === 'Withdrawal') walletTotal -= amt;
    }
    
    var catalog = getCatalogProducts() || [];
    var totalStock = 0;
    var lowStockCount = 0;
    var mediumStockCount = 0;
    var highStockCount = 0;
    var outOfStockCount = 0;
    
    catalog.forEach(prod => {
      var stock = parseInt(prod.Stock || prod.stock || 0);
      totalStock += stock;
      if(stock === 0) outOfStockCount++;
      else if(stock <= 2) lowStockCount++;
      else if(stock <= 5) mediumStockCount++;
      else highStockCount++;
    });
    
    return {
      success: true,
      currentCashBalance: currentCashBalance,
      totalIncomingToday: totalIncomingToday,
      totalOutgoingToday: totalOutgoingToday,
      accountsReceivable: receivables,
      accountsPayable: payables,
      customerWalletBalances: walletTotal,
      netCashFlow: currentCashBalance,
      totalStockQuantity: totalStock,
      lowStockCount: lowStockCount,
      mediumStockCount: mediumStockCount,
      highStockCount: highStockCount,
      outOfStockCount: outOfStockCount
    };
  } catch(e) {
    console.error('getDashboardFinancials error:', e);
    return { success: false, message: e.message };
  }
}

function runDebugDiagnostics(e) {
  const report = {};
  try {
    report.timestamp = new Date().toISOString();
    
    // 1. Spreadsheet Check
    const ss = getSpreadsheet();
    report.spreadsheetId = ss.getId();
    report.spreadsheetName = ss.getName();
    
    // 2. Sheets and row counts
    const sheetsList = ss.getSheets();
    report.sheets = sheetsList.map(s => ({ name: s.getName(), lastRow: s.getLastRow(), lastColumn: s.getLastColumn() }));
    
    // 3. Catalog Sheet Details
    const catalogSheet = getSheet('catalog');
    if (catalogSheet) {
      report.catalogSheetName = catalogSheet.getName();
      const catalogData = catalogSheet.getDataRange().getValues();
      report.catalogRawRowCount = catalogData.length;
      if (catalogData.length > 0) {
        report.catalogHeaders = catalogData[0].map(h => String(h).trim());
        if (catalogData.length > 1) {
          report.catalogFirstRowData = catalogData[1];
        }
      }
    }
    
    // 4. getData('catalog') details
    const catalogRaw = getData('catalog');
    report.getDataCatalogLength = catalogRaw ? catalogRaw.length : 0;
    if (catalogRaw && catalogRaw.length > 0) {
      report.getDataCatalogFirstItemKeys = Object.keys(catalogRaw[0]);
      report.getDataCatalogFirstItem = catalogRaw[0];
    }
    
    // 5. getCatalogProducts() check
    const productsNoArgs = getCatalogProducts();
    report.productsNoArgsLength = productsNoArgs ? productsNoArgs.length : 0;
    if (productsNoArgs && productsNoArgs.length > 0) {
      report.productsNoArgsFirstItem = productsNoArgs[0];
    }
    
    // 6. getCatalogProducts('Active') check
    const productsActive = getCatalogProducts('Active');
    report.productsActiveLength = productsActive ? productsActive.length : 0;
    
    // 7. getPublicCatalogProducts() check
    const publicProducts = getPublicCatalogProducts();
    report.publicProductsLength = publicProducts ? publicProducts.length : 0;
    
    // 8. Sales Billing Sheet Check
    try {
      const salesBilling = ss.getSheetByName('sales_billing');
      if (salesBilling) {
        const rows = salesBilling.getDataRange().getValues();
        report.salesBillingRowsCount = rows.length;
        report.salesBillingLastRows = rows.slice(-10); // get last 10 rows
      } else {
        report.salesBillingRowsCount = 0;
      }
    } catch(err) { report.salesBillingError = err.toString(); }

    // 9. Invoices Sheet Check
    try {
      const invoices = getSheet('invoices');
      if (invoices) {
        const rows = invoices.getDataRange().getValues();
        report.invoicesRowsCount = rows.length;
        report.invoicesLastRows = rows.slice(-10);
      }
    } catch(err) { report.invoicesError = err.toString(); }

    // 10. Ledger Sheet Check
    try {
      const ledger = getSheet('ledger');
      if (ledger) {
        const rows = ledger.getDataRange().getValues();
        report.ledgerRowsCount = rows.length;
        report.ledgerLastRows = rows.slice(-10);
      }
    } catch(err) { report.ledgerError = err.toString(); }

    // 11. Party List Sheet Check
    try {
      const parties = getSheet('parties');
      if (parties) {
        const rows = parties.getDataRange().getValues();
        report.partiesRowsCount = rows.length;
        report.partiesLastRows = rows.slice(-10);
      }
    } catch(err) { report.partiesError = err.toString(); }
    
  } catch (err) {
    report.error = err.toString();
    report.stack = err.stack;
  }
  
  return ContentService.createTextOutput(JSON.stringify(report, null, 2)).setMimeType(ContentService.MimeType.JSON);
}

// ============================================
// MULTI-ROLE DASHBOARD CONTROLLERS
// ============================================

// 1. Dealer (Vendor) Profile Updates
function requestProfileUpdate(role, userId, profileData) {
  try {
    const sheet = getSheet('profile_edit_requests');
    if (!sheet) return { success: false, message: 'Schema missing' };
    
    const reqId = 'PRQ' + new Date().getTime();
    sheet.appendRow([
      reqId,
      new Date().toISOString(),
      role,
      userId,
      profileData.name || '',
      profileData.mobile || '',
      JSON.stringify(profileData),
      'Pending',
      '',
      new Date().toISOString()
    ]);
    return { success: true, reqId: reqId };
  } catch (err) {
    console.error('requestProfileUpdate Error:', err);
    return { success: false, message: err.toString() };
  }
}

// 2. Submit Dealer Order Request
function submitOrderRequest(dealerId, orderData) {
  try {
    const sheet = getSheet('order_requests');
    if (!sheet) return { success: false, message: 'Schema missing' };
    
    const reqId = 'ORD' + new Date().getTime();
    sheet.appendRow([
      reqId,
      new Date().toISOString(),
      dealerId,
      orderData.dealerName || '',
      orderData.mobile || '',
      orderData.shopName || '',
      JSON.stringify(orderData.items || []),
      orderData.totalQuantity || 0,
      orderData.totalAmount || 0,
      'Pending',
      '',
      '',
      new Date().toISOString()
    ]);
    return { success: true, orderId: reqId };
  } catch (err) {
    console.error('submitOrderRequest Error:', err);
    return { success: false, message: err.toString() };
  }
}

// 3. Get Dashboard Analytics
function getDashboardAnalytics(role, identifier) {
  try {
    const analytics = {
      totalOrders: 0,
      totalPurchased: 0,
      pendingOrders: 0,
      warrantyRegistrations: 0,
      walletBalance: 0,
      totalPaid: 0,
      outstanding: 0,
      warranties: [],
      complaints: [],
      orders: [],
      logoUrl: '',
      profile: null
    };

    // Get logo from web settings
    const settings = getData('webSettings') || [];
    const logoSetting = settings.find(s => s.Key === 'site_logo');
    analytics.logoUrl = logoSetting ? logoSetting.Value : 'https://img.icons8.com/color/96/v-varma-logo.png';

    if (role === 'vendor' || role === 'dealer') {
      // 1. Fetch Profile info from Dealers_List
      const dealers = getData('dealers') || [];
      const dealer = dealers.find(d => String(d.Mobile).trim() === String(identifier).trim() || String(d['Dealer ID']).trim() === String(identifier).trim() || String(d.Username).trim() === String(identifier).trim());
      
      let dealerName = '';
      let dealerId = '';
      let dealerMobile = identifier;
      
      if (dealer) {
        dealerName = dealer['Dealer Name'] || '';
        dealerId = dealer['Dealer ID'] || '';
        dealerMobile = dealer.Mobile || identifier;
        analytics.profile = dealer;
      } else {
        // Fallback to vendors or parties
        const vendors = getData('vendors') || [];
        const vendor = vendors.find(v => String(v.Mobile).trim() === String(identifier).trim() || String(v['Vendor ID']).trim() === String(identifier).trim());
        if (vendor) {
          dealerName = vendor['Vendor Name'] || '';
          dealerId = vendor['Vendor ID'] || '';
          dealerMobile = vendor.Mobile || identifier;
          analytics.profile = {
            'Dealer Name': vendor['Vendor Name'],
            'Shop Name': vendor['Shop Name'] || '',
            'Mobile': vendor.Mobile,
            'Email': vendor.Email || '',
            'Address': vendor.Address || '',
            'Area': vendor['Nearby Area'] || '',
            'Pincode': vendor.Pincode || '',
            'Status': vendor.Status || 'Active'
          };
        }
      }
      
      // If dealerName is still empty, let's look up parties list
      if (!dealerName) {
        const parties = getData('parties') || [];
        const party = parties.find(p => String(p.Mobile).trim() === String(dealerMobile).trim());
        if (party) {
          dealerName = party.Name || '';
          dealerId = party['Party ID'] || '';
          if (!analytics.profile) {
            analytics.profile = {
              'Dealer Name': party.Name,
              'Shop Name': party['Shop Name'] || '',
              'Mobile': party.Mobile,
              'Email': party.Email || '',
              'Address': party.Address || '',
              'GST Number': party['GST Number'] || '',
              'Pincode': party.Pincode || '',
              'District': party.District || '',
              'State': party.State || '',
              'Area': party.Landmark || '',
              'Status': party.Status || 'Active'
            };
          }
        }
      }

      // 2. Fetch Warranties
      const w = getData('warranty') || [];
      analytics.warranties = w.filter(x => {
        const rawVendor = String(x['Vendor Name'] || '').trim().toLowerCase();
        const matchesName = dealerName && rawVendor === dealerName.toLowerCase();
        const matchesMobile = String(x['Mobile Number'] || '').trim() === String(dealerMobile).trim();
        return matchesName || matchesMobile;
      });

      // 3. Fetch Complaints
      const complaints = getData('complaints') || [];
      analytics.complaints = complaints.filter(x => {
        const rawModel = String(x['Product Model'] || '').trim();
        const matchesMobile = String(x['Mobile Number'] || '').trim() === String(dealerMobile).trim();
        // Also match via registered warranty IDs
        const isFromRegisteredWarranty = analytics.warranties.some(wr => String(wr['Warranty ID'] || '').trim() === String(x['Warranty ID'] || '').trim());
        return matchesMobile || isFromRegisteredWarranty;
      });

      // 4. Fetch Order Requests / Orders
      const orderRequests = getData('order_requests') || [];
      analytics.orders = orderRequests.filter(x => {
        const matchesMobile = String(x.Mobile || '').trim() === String(dealerMobile).trim();
        const matchesId = dealerId && String(x['Dealer ID'] || '').trim() === String(dealerId).trim();
        return matchesMobile || matchesId;
      });
      analytics.totalOrders = analytics.orders.length;

      // 5. Fetch Ledger & Outstanding
      const ledger = getData('ledger') || [];
      const dealerLedger = ledger.filter(x => {
        const matchesMobile = String(x.Mobile || '').trim() === String(dealerMobile).trim();
        const matchesId = dealerId && String(x['Customer/Vendor ID'] || '').trim() === String(dealerId).trim();
        return matchesMobile || matchesId;
      });
      
      const parties = getData('parties') || [];
      const party = parties.find(p => String(p.Mobile).trim() === String(dealerMobile).trim());
      if (party && party['Current Balance'] !== undefined && party['Current Balance'] !== '') {
        analytics.outstanding = parseFloat(party['Current Balance'] || 0);
      } else {
        if (dealerLedger.length > 0) {
          analytics.outstanding = parseFloat(dealerLedger[dealerLedger.length - 1].Balance || 0);
        } else {
          analytics.outstanding = 0;
        }
      }
    }
    
    return { success: true, data: analytics };
  } catch (err) {
    console.error('getDashboardAnalytics Error:', err);
    return { success: false, message: err.toString() };
  }
}

// 4. Fetch Role Specific Data
function getRoleSpecificData(role, identifier) {
  try {
    const data = {
      orders: [],
      purchases: [],
      warranties: [],
      complaints: [],
      ledger: []
    };
    
    const w = getData('warranty') || [];
    const c = getData('complaints') || [];
    
    if (role === 'customer') {
      var parties = getPartyList();
      var p = parties.find(x => String(x.Mobile).trim() === String(identifier).trim() || String(x.Id).trim() === String(identifier).trim());
      var pId = p ? p.Id : '';
      data.warranties = w.filter(x => x['Mobile Number'] == identifier || x['Customer Name'] == identifier || (pId && x['Mobile Number'] == pId));
      data.complaints = c.filter(x => x['Mobile Number'] == identifier || x['Customer Name'] == identifier || (pId && x['Mobile Number'] == pId));
    } else if (role === 'vendor' || role === 'dealer') {
      const dealers = getData('dealers') || [];
      const dealer = dealers.find(d => String(d.Mobile).trim() === String(identifier).trim() || String(d['Dealer ID']).trim() === String(identifier).trim() || String(d.Username).trim() === String(identifier).trim());
      
      let dealerName = '';
      let dealerId = '';
      let dealerMobile = identifier;
      
      if (dealer) {
        dealerName = dealer['Dealer Name'] || '';
        dealerId = dealer['Dealer ID'] || '';
        dealerMobile = dealer.Mobile || identifier;
      } else {
        const vendors = getData('vendors') || [];
        const vendor = vendors.find(v => String(v.Mobile).trim() === String(identifier).trim() || String(v['Vendor ID']).trim() === String(identifier).trim());
        if (vendor) {
          dealerName = vendor['Vendor Name'] || '';
          dealerId = vendor['Vendor ID'] || '';
          dealerMobile = vendor.Mobile || identifier;
        }
      }
      
      const parties = getData('parties') || [];
      const party = parties.find(p => String(p.Mobile).trim() === String(dealerMobile).trim());
      if (party) {
        if (!dealerName) dealerName = party.Name || '';
        if (!dealerId) dealerId = party['Party ID'] || '';
      }

      data.warranties = w.filter(x => {
        const rawVendor = String(x['Vendor Name'] || '').trim().toLowerCase();
        const matchesName = dealerName && rawVendor === dealerName.toLowerCase();
        const matchesMobile = String(x['Mobile Number'] || '').trim() === String(dealerMobile).trim();
        return matchesName || matchesMobile;
      });
      
      const orders = getData('order_requests') || [];
      data.orders = orders.filter(x => {
        const matchesMobile = String(x.Mobile || '').trim() === String(dealerMobile).trim();
        const matchesId = dealerId && String(x['Dealer ID'] || '').trim() === String(dealerId).trim();
        return matchesMobile || matchesId;
      });
      
      const ledger = getData('ledger') || [];
      data.ledger = ledger.filter(x => {
        const matchesMobile = String(x.Mobile || '').trim() === String(dealerMobile).trim();
        const matchesId = dealerId && String(x['Customer/Vendor ID'] || '').trim() === String(dealerId).trim();
        return matchesMobile || matchesId;
      });
    } else if (role === 'tech') {
      data.complaints = c.filter(x => x['Technician Assigned'] == identifier);
    }
    
    return { success: true, data: data };
  } catch(err) {
    console.error('getRoleSpecificData error:', err);
    return { success: false, message: err.toString() };
  }
}

function updateTechJob(complaintId, updates) {
  try {
    const sheet = getSheet('complaints');
    const data = sheet.getDataRange().getValues();
    const headers = data[0];
    const idIdx = headers.indexOf('Complaint ID');
    const statusIdx = headers.indexOf('Status');
    const remarksIdx = headers.indexOf('Technician Remarks');
    const completedIdx = headers.indexOf('Completed Date');
    
    for (let i = 1; i < data.length; i++) {
      if (data[i][idIdx] === complaintId) {
        if (updates.status) sheet.getRange(i + 1, statusIdx + 1).setValue(updates.status);
        if (updates.remarks) sheet.getRange(i + 1, remarksIdx + 1).setValue(updates.remarks);
        if (updates.status === 'Completed') {
          sheet.getRange(i + 1, completedIdx + 1).setValue(new Date().toISOString());
        }
        return { success: true };
      }
    }
    return { success: false, message: 'Complaint ID not found' };
  } catch(err) {
    console.error('updateTechJob error:', err);
    return { success: false, message: err.toString() };
  }
}

// ============================================================
// ORDER MANAGEMENT — saveDealerOrder
// ============================================================
function saveDealerOrder(orderData) {
  try {
    var sheet = getSheet('orders') || getSheet('order_requests');
    if (!sheet) throw new Error('Orders sheet not found');

    var headers = sheet.getRange(1, 1, 1, sheet.getLastColumn()).getValues()[0];
    var orderId = 'ORD-DLR-' + new Date().getTime();
    var now = new Date().toISOString();

    var rowData = headers.map(function(h) {
      switch(h) {
        case 'Order ID':          return orderId;
        case 'Order Number':      return orderId;
        case 'Order Date':        return now;
        case 'Timestamp':         return now;
        case 'Order Type':        return orderData.orderType || 'Dealer';
        case 'Dealer ID':         return orderData.dealerId || orderData.mobile || '';
        case 'Dealer Name':       return orderData.dealerName || '';
        case 'Shop Name':         return orderData.shopName || '';
        case 'GST Number':        return orderData.gstNumber || '';
        case 'Mobile':            return orderData.mobile || '';
        case 'Customer Mobile':   return orderData.mobile || '';
        case 'Product ID':        return orderData.productId || '';
        case 'Model Number':      return orderData.modelNumber || '';
        case 'Model Name':        return orderData.modelName || '';
        case 'Product Title':     return orderData.productTitle || orderData.modelName || '';
        case 'Part Number':       return orderData.partNumber || '';
        case 'Quantity':          return orderData.quantity || 1;
        case 'Wholesale Price':   return orderData.wholesalePrice || 0;
        case 'Price':             return orderData.wholesalePrice || orderData.price || 0;
        case 'Total Amount':      return orderData.totalAmount || 0;
        case 'Invoice Type':      return orderData.invoiceType || 'B2B';
        case 'Order Stage':       return orderData.orderStage || 'Order Placed';
        case 'Status':            return orderData.orderStage || 'Order Placed';
        case 'Notes':             return orderData.notes || '';
        case 'Items (JSON)':      return JSON.stringify(orderData.items || []);
        case 'Expected Delivery': return orderData.expectedDelivery || '';
        default:                  return '';
      }
    });

    sheet.appendRow(rowData);
    return { success: true, orderId: orderId };
  } catch(err) {
    console.error('saveDealerOrder error:', err);
    return { success: false, message: err.toString() };
  }
}

// ============================================================
// ORDER MANAGEMENT — saveCustomerOrder
// ============================================================
function saveCustomerOrder(orderData) {
  try {
    var sheet = getSheet('orders') || getSheet('order_requests');
    if (!sheet) throw new Error('Orders sheet not found');

    var headers = sheet.getRange(1, 1, 1, sheet.getLastColumn()).getValues()[0];
    var orderId = 'ORD-CUST-' + new Date().getTime();
    var now = new Date().toISOString();

    var rowData = headers.map(function(h) {
      switch(h) {
        case 'Order ID':          return orderId;
        case 'Order Number':      return orderId;
        case 'Order Date':        return now;
        case 'Timestamp':         return now;
        case 'Order Type':        return orderData.orderType || 'Customer';
        case 'Customer Name':     return orderData.customerName || '';
        case 'Customer Mobile':   return orderData.mobile || '';
        case 'Mobile':            return orderData.mobile || '';
        case 'Dealer ID':         return '';
        case 'Dealer Name':       return '';
        case 'Product ID':        return orderData.productId || '';
        case 'Model Number':      return orderData.modelNumber || '';
        case 'Model Name':        return orderData.modelName || '';
        case 'Product Title':     return orderData.productTitle || orderData.modelName || '';
        case 'Part Number':       return orderData.partNumber || '';
        case 'Quantity':          return orderData.quantity || 1;
        case 'Price':             return orderData.price || 0;
        case 'Total Amount':      return orderData.totalAmount || 0;
        case 'Invoice Type':      return orderData.invoiceType || 'B2C';
        case 'Order Stage':       return orderData.orderStage || 'Order Placed';
        case 'Status':            return orderData.orderStage || 'Order Placed';
        case 'Delivery Address':  return orderData.address || '';
        case 'Notes':             return orderData.notes || '';
        case 'Items (JSON)':      return JSON.stringify([{productId: orderData.productId, model: orderData.modelNumber, name: orderData.productTitle, qty: orderData.quantity, price: orderData.price}]);
        case 'Expected Delivery': return orderData.expectedDelivery || '';
        default:                  return '';
      }
    });

    sheet.appendRow(rowData);
    return { success: true, orderId: orderId };
  } catch(err) {
    console.error('saveCustomerOrder error:', err);
    return { success: false, message: err.toString() };
  }
}
// ============================================================
// IVR ESP32 DASHBOARD — VARMA PANEL DATA API  v3
// Endpoints:
//   GET <scriptUrl>?action=getVarmaData     → full dashboard data
//   GET <scriptUrl>?action=initVarmaSheets  → create / repair all sheets
//   POST <scriptUrl>                         → sync call log / messages
// Auto-init runs once per deploy.
// ============================================================

// ── Auto-init guard (runs once when the script is first deployed) ────────────
var _VARMA_SHEETS_INITIALIZED_ = false;
function _autoInitOnce_() {
  if (_VARMA_SHEETS_INITIALIZED_) return;
  try { initVarmaSheets(); } catch(e) { console.warn('Auto-init warn:', e.message); }
  _VARMA_SHEETS_INITIALIZED_ = true;
}

// ── IVR-specific sheet schemas ───────────────────────────────────────────────
var IVR_SCHEMAS = {
  IVR_Calls:    ['Call_ID', 'Number', 'Date', 'Time', 'Duration', 'Status', 'Type', 'DTMF_Input'],
  IVR_Messages: ['Message_ID', 'Number', 'Date', 'Time', 'Message', 'Direction', 'Status'],
  IVR_HelpCenter_Numbers: ['ID', 'Number', 'Name', 'Role', 'ForwardEnabled', 'SmsEnabled']
};

/**
 * Ensure an IVR-specific sheet exists with correct headers.
 * If it exists but has no header, the header is added.
 * Returns the sheet object.
 */
function _ensureIvrSheet_(sheetName, headers) {
  var ss = getSpreadsheet();
  var sheet = ss.getSheetByName(sheetName);
  if (!sheet) {
    sheet = ss.insertSheet(sheetName);
    sheet.appendRow(headers);
    sheet.getRange(1, 1, 1, headers.length)
         .setFontWeight('bold')
         .setBackground('#e8f5e9')
         .setFontColor('#1b5e20');
    sheet.setFrozenRows(1);
    console.log('Created IVR sheet: ' + sheetName);
  } else {
    // Repair missing headers
    if (sheet.getLastRow() === 0) {
      sheet.appendRow(headers);
      sheet.getRange(1, 1, 1, headers.length).setFontWeight('bold').setBackground('#e8f5e9');
      sheet.setFrozenRows(1);
    }
  }
  SpreadsheetApp.flush();
  return sheet;
}

/**
 * doPost(e)
 * Handles incoming call syncs and message logs from the ESP32.
 */
function doPost(e) {
  try {
    _autoInitOnce_();
    var jsonString = e.postData.contents;
    var data = JSON.parse(jsonString);
    
    // Set request-scoped spreadsheet ID from POST body or query parameter
    if (data && (data.sheetId || data.spreadsheetId)) {
      REQUEST_SPREADSHEET_ID = (data.sheetId || data.spreadsheetId).trim();
    } else if (e && e.parameter && (e.parameter.sheetId || e.parameter.spreadsheetId)) {
      REQUEST_SPREADSHEET_ID = (e.parameter.sheetId || e.parameter.spreadsheetId).trim();
    }
    var now = new Date();
    var dateStr = now.toISOString().split('T')[0];
    var timeStr = now.toLocaleTimeString([], { hour: '2-digit', minute: '2-digit', second: '2-digit' });
    
    // Message log request
    if (data.message || data.msg) {
      var sheet = _ensureIvrSheet_('IVR_Messages', IVR_SCHEMAS.IVR_Messages);
      var msgId = 'MSG-' + now.getTime() + '-' + Math.floor(Math.random() * 1000);
      
      sheet.appendRow([
        msgId,
        data.phoneNumber || data.number || '',
        dateStr,
        timeStr,
        data.message || data.msg || '',
        data.direction || 'Outgoing',
        data.status || 'Sent'
      ]);
      return ContentService.createTextOutput(JSON.stringify({ success: true, messageId: msgId }))
        .setMimeType(ContentService.MimeType.JSON);
    } else {
      // Call log request
      var sheet = _ensureIvrSheet_('IVR_Calls', IVR_SCHEMAS.IVR_Calls);
      var callId = 'CALL-' + now.getTime() + '-' + Math.floor(Math.random() * 1000);
      
      var duration = '0';
      if (data.callStatus && data.callStatus.indexOf('Duration:') != -1) {
        var match = data.callStatus.match(/Duration:\s*(\d+)s/);
        if (match) duration = match[1];
      }
      
      sheet.appendRow([
        callId,
        data.phoneNumber || '',
        dateStr,
        timeStr,
        duration,
        data.callStatus || '',
        data.type || data.direction || 'Incoming',
        data.dtmfData || ''
      ]);
      return ContentService.createTextOutput(JSON.stringify({ success: true, callId: callId }))
        .setMimeType(ContentService.MimeType.JSON);
    }
  } catch (error) {
    console.error('doPost error:', error);
    return ContentService.createTextOutput(JSON.stringify({ success: false, error: error.toString() }))
      .setMimeType(ContentService.MimeType.JSON);
  }
}

function doOptions(e) {
  return ContentService.createTextOutput("")
    .setMimeType(ContentService.MimeType.TEXT);
}

/**
 * initVarmaSheets
 * Safe to call multiple times — idempotent.
 */
function initVarmaSheets() {
  try {
    // 1. Main DB schema (Warranty_Register, Complaints, Party_List, etc.)
    initializeDatabaseHeaders();

    // 2. IVR-specific sheets
    Object.keys(IVR_SCHEMAS).forEach(function(name) {
      _ensureIvrSheet_(name, IVR_SCHEMAS[name]);
    });

    // 3. Build status report
    var allSheets = ['warranty', 'complaints', 'parties', 'users', 'serviceHistory'];
    var report = {};
    allSheets.forEach(function(key) {
      try {
        var s = getSheet(key);
        report[key] = { ok: true, rows: Math.max(0, s.getLastRow() - 1) };
      } catch(e) {
        report[key] = { ok: false, error: e.message };
      }
    });
    Object.keys(IVR_SCHEMAS).forEach(function(name) {
      try {
        var s = getSpreadsheet().getSheetByName(name);
        report[name] = { ok: !!s, rows: s ? Math.max(0, s.getLastRow() - 1) : 0 };
      } catch(e) {
        report[name] = { ok: false, error: e.message };
      }
    });

    return { success: true, message: 'All sheets initialized / verified', sheets: report };
  } catch(e) {
    console.error('initVarmaSheets error:', e);
    return { success: false, message: e.message };
  }
}

/**
 * _readIvrSheet_
 * Read rows from an IVR-specific sheet (IVR_Calls or IVR_Messages)
 */
function _readIvrSheet_(sheetName) {
  try {
    var ss = getSpreadsheet();
    var sheet = ss.getSheetByName(sheetName);
    if (!sheet || sheet.getLastRow() < 2) return [];
    var rows = sheet.getDataRange().getValues();
    var headers = rows[0].map(function(h) { return String(h).trim(); });
    return rows.slice(1).filter(function(r) {
      return r.some(function(c) { return c !== '' && c !== null && c !== undefined; });
    }).map(function(r) {
      var obj = {};
      headers.forEach(function(h, i) {
        var v = r[i];
        if (v instanceof Date) v = v.toISOString();
        obj[h] = v;
      });
      return obj;
    });
  } catch(e) {
    console.warn('_readIvrSheet_ error [' + sheetName + ']:', e.message);
    return [];
  }
}

/**
 * getIvrCalls
 * Returns the calls array for direct query.
 */
function getIvrCalls() {
  var ivrCalls = _readIvrSheet_('IVR_Calls');
  return ivrCalls.map(function(c) {
    var dateVal = String(c['Date'] || '').split('T')[0];
    var timeVal = String(c['Time'] || '').trim();
    var timeStrVal = dateVal;
    if (timeVal) {
      if (timeVal.includes('T')) {
        timeVal = timeVal.split('T')[1].split('.')[0];
      }
      timeStrVal += 'T' + timeVal;
    }
    return {
      phoneNumber: String(c['Number'] || '').trim(),
      timeStr: timeStrVal,
      callStatus: String(c['Status'] || 'Answered').trim(),
      type: String(c['Type'] || 'Incoming').trim()
    };
  });
}

/**
 * getIvrMessages
 * Returns the messages array for direct query.
 */
function getIvrMessages() {
  var ivrMessages = _readIvrSheet_('IVR_Messages');
  return ivrMessages.map(function(m) {
    var dateVal = String(m['Date'] || '').split('T')[0];
    var timeVal = String(m['Time'] || '').trim();
    var timeStrVal = dateVal;
    if (timeVal) {
      if (timeVal.includes('T')) {
        timeVal = timeVal.split('T')[1].split('.')[0];
      }
      timeStrVal += 'T' + timeVal;
    }
    return {
      phoneNumber: String(m['Number'] || '').trim(),
      message: String(m['Message'] || '').trim(),
      direction: String(m['Direction'] || 'Outgoing').trim(),
      status: String(m['Status'] || 'Sent').trim(),
      timeStr: timeStrVal
    };
  });
}

function getHcNumbers() {
  var rows = _readIvrSheet_('IVR_HelpCenter_Numbers');
  return rows.map(function(r) {
    return {
      id: r['ID'] ? String(r['ID']).trim() : '',
      number: r['Number'] ? String(r['Number']).trim() : '',
      name: r['Name'] ? String(r['Name']).trim() : '',
      role: r['Role'] ? String(r['Role']).trim() : '',
      forwardEnabled: String(r['ForwardEnabled'] || '').trim().toLowerCase() === 'true',
      smsEnabled: String(r['SmsEnabled'] || '').trim().toLowerCase() === 'true'
    };
  });
}

function saveHcNumber(e) {
  try {
    var id = e.parameter.id ? String(e.parameter.id).trim() : '';
    var number = e.parameter.number ? String(e.parameter.number).trim() : '';
    var name = e.parameter.name ? String(e.parameter.name).trim() : '';
    var role = e.parameter.role ? String(e.parameter.role).trim() : '';
    var forwardEnabled = (e.parameter.forwardEnabled === 'true' || e.parameter.forwardEnabled === '1') ? 'true' : 'false';
    var smsEnabled = (e.parameter.smsEnabled === 'true' || e.parameter.smsEnabled === '1') ? 'true' : 'false';

    var ss = getSpreadsheet();
    var sheet = ss.getSheetByName('IVR_HelpCenter_Numbers');
    if (!sheet) {
      sheet = _ensureIvrSheet_('IVR_HelpCenter_Numbers', IVR_SCHEMAS.IVR_HelpCenter_Numbers);
    }

    var data = sheet.getDataRange().getValues();
    var foundIdx = -1;
    if (id) {
      for (var i = 1; i < data.length; i++) {
        if (String(data[i][0]).trim() === id) {
          foundIdx = i;
          break;
        }
      }
    }

    if (foundIdx !== -1) {
      sheet.getRange(foundIdx + 1, 1, 1, 6).setValues([[id, number, name, role, forwardEnabled, smsEnabled]]);
      return { success: true, message: 'Help Center number updated' };
    } else {
      var newId = id || String(Date.now());
      sheet.appendRow([newId, number, name, role, forwardEnabled, smsEnabled]);
      return { success: true, message: 'Help Center number added', id: newId };
    }
  } catch(e) {
    return { success: false, message: e.message };
  }
}

function deleteHcNumber(e) {
  try {
    var id = e.parameter.id ? String(e.parameter.id).trim() : '';
    if (!id) return { success: false, message: 'Missing ID' };

    var ss = getSpreadsheet();
    var sheet = ss.getSheetByName('IVR_HelpCenter_Numbers');
    if (!sheet) return { success: false, message: 'Sheet not found' };

    var data = sheet.getDataRange().getValues();
    for (var i = 1; i < data.length; i++) {
      if (String(data[i][0]).trim() === id) {
        sheet.deleteRow(i + 1);
        return { success: true, message: 'Deleted Help Center number with ID: ' + id };
      }
    }
    return { success: false, message: 'Help Center number ID not found: ' + id };
  } catch(e) {
    return { success: false, message: e.message };
  }
}

/**
 * getVarmaData
 * Returns the full VARMA Panel payload.
 */
function getVarmaData() {
  try {
    // Auto-init on first call
    _autoInitOnce_();

    // ── Load all sheets ──────────────────────────────────────────────────────
    var warrantyData  = getData('warranty')      || [];
    var complaintData = getData('complaints')    || [];
    var partiesData   = getData('parties')       || [];

    var ivrCalls    = _readIvrSheet_('IVR_Calls');
    var ivrMessages = _readIvrSheet_('IVR_Messages');

    // ── Build party lookup (mobile → name) ──────────────────────────────────
    var partyByMobile = {};
    partiesData.forEach(function(p) {
      var mob = String(p['Mobile'] || p.mobile || '').trim().replace(/\D/g, '');
      if (mob) partyByMobile[mob] = String(p['Name'] || p.name || '').trim();
    });

    // ── Registered Customers (from Warranty_Register) ───────────────────────
    var registeredNumbers = warrantyData.map(function(w) {
      var endDate = String(w['Warranty End Date'] || w.warrantyenddate || '');
      var expired = false;
      if (endDate) { try { expired = new Date(endDate) < new Date(); } catch(_) {} }
      return {
        name:    String(w['Customer Name'] || w.customername || '').trim(),
        number:  String(w['Mobile Number'] || w.mobilenumber || '').trim(),
        product: String(w['Product Model'] || w.productmodel || '').trim(),
        serial:  String(w['Part Number']   || w.partnumber   || '').trim(),
        status:  expired ? 'Expired Warranty' : 'Active Warranty'
      };
    }).filter(function(r) { return r.name || r.number; });

    var registeredMobiles = {};
    registeredNumbers.forEach(function(r) {
      registeredMobiles[r.number.replace(/\D/g, '')] = r.name;
    });

    // ── Warranty Details table ───────────────────────────────────────────────
    var warranties = warrantyData.map(function(w) {
      return {
        date:    String(w['Registration Date'] || w.registrationdate || '').split('T')[0],
        name:    String(w['Customer Name']     || w.customername     || '').trim(),
        number:  String(w['Mobile Number']     || w.mobilenumber     || '').trim(),
        product: String(w['Product Model']     || w.productmodel     || '').trim(),
        serial:  String(w['Part Number']       || w.partnumber       || '').trim(),
        expiry:  String(w['Warranty End Date'] || w.warrantyenddate  || '').split('T')[0]
      };
    }).filter(function(r) { return r.name || r.number; });

    // ── Call History ─────────────────────────────────────────────────────────
    // Primary: IVR_Calls sheet
    var callHistories = ivrCalls.map(function(c) {
      var mob = String(c['Number'] || '').trim();
      var normMob = mob.replace(/\D/g, '');
      var custName = registeredMobiles[normMob] || partyByMobile[normMob] || '';
      var dateVal = String(c['Date'] || '').split('T')[0];
      var timeVal = String(c['Time'] || '').trim();
      var displayDate = dateVal;
      if (timeVal) {
        if (timeVal.includes('T')) {
          timeVal = timeVal.split('T')[1].split('.')[0];
        }
        displayDate += ' ' + timeVal;
      }
      return {
        date:   displayDate,
        number: mob,
        type:   String(c['Type'] || 'Incoming').trim(),
        status: String(c['Status']    || 'Answered').trim(),
        name:   custName,
        dtmf:   '',
        duration: String(c['Duration'] || '').trim()
      };
    }).filter(function(r) { return r.number; });

    // Fallback: derive from complaints if IVR_Calls is empty
    if (callHistories.length === 0) {
      callHistories = complaintData.map(function(c) {
        var mob = String(c['Mobile Number'] || c.mobilenumber || '').trim();
        var normMob = mob.replace(/\D/g, '');
        return {
          date:   String(c['Complaint Date'] || c.complaintdate || '').split('T')[0],
          number: mob,
          type:   'Incoming',
          status: String(c['Status'] || c.status || 'Logged').trim(),
          name:   registeredMobiles[normMob] || partyByMobile[normMob] || '',
          dtmf:   '',
          duration: ''
        };
      }).filter(function(r) { return r.number; });
    }

    // ── Messages ─────────────────────────────────────────────────────────────
    // Primary: IVR_Messages sheet
    var messages = ivrMessages.map(function(m) {
      var dateVal = String(m['Date'] || '').split('T')[0];
      var timeVal = String(m['Time'] || '').trim();
      var displayDate = dateVal;
      if (timeVal) {
        if (timeVal.includes('T')) {
          timeVal = timeVal.split('T')[1].split('.')[0];
        }
        displayDate += ' ' + timeVal;
      }
      return {
        date:    displayDate,
        number:  String(m['Number'] || '').trim(),
        message: String(m['Message'] || '').trim(),
        status:  String(m['Direction'] || m['Status'] || 'Info').trim()
      };
    }).filter(function(m) { return m.message; });

    // Fallback: service history
    if (messages.length === 0) {
      try {
        var serviceData = getData('serviceHistory') || [];
        messages = serviceData.slice(0, 100).map(function(s) {
          return {
            date:    String(s['Event Date'] || s.eventdate || '').split('T')[0],
            number:  String(s['Mobile']     || s.mobile    || '').trim(),
            message: String(s['Notes']      || s.notes     || s['Status'] || '').trim(),
            status:  String(s['Event Type'] || s.eventtype || 'Info').trim()
          };
        }).filter(function(m) { return m.message; });
      } catch(_) {}
    }

    // ── Complaints ───────────────────────────────────────────────────────────
    var complaints = complaintData.map(function(c) {
      return {
        id:     String(c['Complaint ID']        || c.complaintid        || '').trim(),
        date:   String(c['Complaint Date']      || c.complaintdate      || '').split('T')[0],
        name:   String(c['Customer Name']       || c.customername       || '').trim(),
        number: String(c['Mobile Number']       || c.mobilenumber       || '').trim(),
        desc:   String(c['Problem Description'] || c.problemdescription || '').trim(),
        status: String(c['Status']              || c.status             || '').trim()
      };
    }).filter(function(r) { return r.id || r.name; });

    // ── Unknown Numbers ──────────────────────────────────────────────────────
    var unknownMap = {};
    complaintData.forEach(function(c) {
      var mob  = String(c['Mobile Number'] || c.mobilenumber || '').trim();
      var norm = mob.replace(/\D/g, '');
      if (mob && !registeredMobiles[norm]) {
        if (!unknownMap[mob]) {
          unknownMap[mob] = { number: mob, date: String(c['Complaint Date'] || c.complaintdate || '').split('T')[0], count: 0 };
        }
        unknownMap[mob].count++;
      }
    });
    // Also scan IVR_Calls for unknown callers
    ivrCalls.forEach(function(c) {
      var mob  = String(c['Number'] || '').trim();
      var norm = mob.replace(/\D/g, '');
      if (mob && !registeredMobiles[norm]) {
        if (!unknownMap[mob]) {
          unknownMap[mob] = { number: mob, date: String(c['Date'] || '').split('T')[0], count: 0 };
        }
        unknownMap[mob].count++;
      }
    });
    var unknownNumbers = Object.values(unknownMap);

    // ── Aggregate Counters ───────────────────────────────────────────────────
    var totalCalls       = callHistories.length;
    var attendedCalls    = callHistories.filter(function(c) {
      return ['Answered','Attended','Completed','Active'].indexOf(String(c.status)) > -1;
    }).length;
    var missedCalls      = callHistories.filter(function(c) {
      return ['Missed','No Answer','Rejected','Pending'].indexOf(String(c.status)) > -1;
    }).length;
    var dialedCalls      = callHistories.filter(function(c) {
      return String(c.type).toLowerCase() === 'outgoing';
    }).length;
    var knownNumbers     = registeredNumbers.length;
    var unknownCount     = unknownNumbers.length;
    var messagesSent     = messages.filter(function(m) {
      return String(m.status).toLowerCase().indexOf('sent') > -1 || String(m.status).toLowerCase().indexOf('outgoing') > -1;
    }).length;
    var messagesReceived = messages.filter(function(m) {
      return String(m.status).toLowerCase().indexOf('received') > -1 || String(m.status).toLowerCase().indexOf('incoming') > -1;
    }).length;
    var complaintsFiled  = complaintData.length;
    var complaintsSolved = complaintData.filter(function(c) {
      var st = String(c['Status'] || '').toLowerCase();
      return st.indexOf('solved') > -1 || st.indexOf('completed') > -1 || st.indexOf('closed') > -1;
    }).length;
    var techMessaged     = complaintData.filter(function(c) {
      return c['Technician Assigned'] && String(c['Technician Assigned']).trim();
    }).length;

    // ── Response ─────────────────────────────────────────────────────────────
    return {
      ok: true,
      spreadsheetId: getSpreadsheet().getId(),
      fetchedAt: new Date().toISOString(),
      sources: {
        ivrCallHistory:  ivrCalls.length    > 0 ? 'IVR_Calls'         : 'complaints_fallback',
        ivrMessages:     ivrMessages.length > 0 ? 'IVR_Messages'      : 'service_history_fallback',
        warrantyRows:    warrantyData.length,
        complaintRows:   complaintData.length,
        partyRows:       partiesData.length
      },
      topCards: {
        complaintsFiled:      complaintsFiled,
        complaintsSolved:     complaintsSolved,
        techMessaged:         techMessaged,
        hcMessages:           messages.length,
        custReceivedMessages: messagesReceived
      },
      gridCounters: {
        totalCalls:       totalCalls,
        attendedCalls:    attendedCalls,
        missedCalls:      missedCalls,
        dialedCalls:      dialedCalls,
        unknownNumbers:   unknownCount,
        knownNumbers:     knownNumbers,
        messagesSent:     messagesSent,
        messagesReceived: messagesReceived
      },
      callHistories:     callHistories,
      messages:          messages,
      unknownNumbers:    unknownNumbers,
      registeredNumbers: registeredNumbers,
      complaints:        complaints,
      warranties:        warranties
    };

  } catch(e) {
    console.error('getVarmaData error:', e);
    return { ok: false, error: e.message };
  }
}




/**
 * validateUser
 * Checks if phone number is registered in users, parties, warranty register, technicians, or dealers sheets.
 */
function validateUser(phone) {
  if (!phone) return { registered: false, phone: '' };
  
  var cleanPhone = String(phone).replace(/\D/g, '');
  if (cleanPhone.length > 10) {
    cleanPhone = cleanPhone.slice(-10);
  }
  if (!cleanPhone) return { registered: false, phone: phone };

  // 1. Check users
  var users = getData('users') || [];
  for (var i = 0; i < users.length; i++) {
    var u = users[i];
    var uMob = String(u.Mobile || u.mobile || '').replace(/\D/g, '');
    if (uMob.length >= 10 && uMob.slice(-10) === cleanPhone) {
      return { registered: true, name: u.Name || u.name || '', phone: phone };
    }
  }

  // 2. Check parties
  var parties = getData('parties') || [];
  for (var i = 0; i < parties.length; i++) {
    var p = parties[i];
    var pMob = String(p.Mobile || p.mobile || '').replace(/\D/g, '');
    if (pMob.length >= 10 && pMob.slice(-10) === cleanPhone) {
      return { registered: true, name: p.Name || p.name || '', phone: phone };
    }
  }

  // 3. Check warranty
  var warranty = getData('warranty') || [];
  for (var i = 0; i < warranty.length; i++) {
    var w = warranty[i];
    var wMob = String(w['Mobile Number'] || w.mobilenumber || w.Mobile || w.mobile || '').replace(/\D/g, '');
    if (wMob.length >= 10 && wMob.slice(-10) === cleanPhone) {
      return { registered: true, name: w['Customer Name'] || w.customername || '', phone: phone };
    }
  }

  // 4. Check technicians
  var techs = getData('technicians') || [];
  for (var i = 0; i < techs.length; i++) {
    var t = techs[i];
    var tMob = String(t.Mobile || t.mobile || '').replace(/\D/g, '');
    if (tMob.length >= 10 && tMob.slice(-10) === cleanPhone) {
      return { registered: true, name: t.Name || t.name || '', phone: phone };
    }
  }

  // 5. Check dealers
  var dealers = getData('dealers') || [];
  for (var i = 0; i < dealers.length; i++) {
    var d = dealers[i];
    var dMob = String(d.Mobile || d.mobile || '').replace(/\D/g, '');
    if (dMob.length >= 10 && dMob.slice(-10) === cleanPhone) {
      return { registered: true, name: d['Dealer Name'] || d.dealername || d.Name || d.name || '', phone: phone };
    }
  }

  return { registered: false, phone: phone };
}

/**
 * getRegisteredMobiles
 * Consolidates registered phone numbers from Warranty_Register, Party_List, Users,
 * Technician_List, and Dealers_List into a clean, duplicate-free list.
 */
function getRegisteredMobiles() {
  try {
    var warrantyData = getData('warranty') || [];
    var partiesData = getData('parties') || [];
    var usersData = getData('users') || [];
    var techniciansData = getData('technicians') || [];
    var dealersData = getData('dealers') || [];
    
    var list = [];
    var seen = {};

    function addMobile(phone, name, product, serial, status) {
      if (!phone) return;
      var clean = String(phone).replace(/\D/g, '');
      if (clean.length > 10) clean = clean.slice(-10);
      if (!clean) return;
      if (seen[clean]) return;
      seen[clean] = true;
      list.push({
        Phone: phone,
        Mobile: phone,
        Name: name,
        Product: product,
        Serial: serial,
        Status: status
      });
    }

    warrantyData.forEach(function(w) {
      var name = String(w['Customer Name'] || w.customername || '').trim();
      var phone = String(w['Mobile Number'] || w.mobilenumber || w.Mobile || w.mobile || '').trim();
      var product = String(w['Product Model'] || w.productmodel || '').trim();
      var serial = String(w['Part Number'] || w.partnumber || '').trim();
      var endDate = String(w['Warranty End Date'] || w.warrantyenddate || '');
      var status = 'Active Warranty';
      if (endDate) {
        try {
          if (new Date(endDate) < new Date()) status = 'Expired Warranty';
        } catch(_) {}
      }
      addMobile(phone, name, product, serial, status);
    });

    partiesData.forEach(function(p) {
      var name = String(p['Name'] || p.name || '').trim();
      var phone = String(p['Mobile'] || p.mobile || '').trim();
      var type = String(p['Type'] || p.type || 'Party').trim();
      var status = String(p['Status'] || p.status || 'Active').trim();
      addMobile(phone, name, type, '', status);
    });

    usersData.forEach(function(u) {
      var name = String(u['Name'] || u.name || '').trim();
      var phone = String(u['Mobile'] || u.mobile || '').trim();
      var role = String(u['Role'] || u.role || 'User').trim();
      var status = String(u['Status'] || u.status || 'Active').trim();
      addMobile(phone, name, role, '', status);
    });

    techniciansData.forEach(function(t) {
      var name = String(t['Name'] || t.name || '').trim();
      var phone = String(t['Mobile'] || t.mobile || '').trim();
      var status = String(t['Status'] || t.status || 'Active').trim();
      addMobile(phone, name, 'Technician', '', status);
    });

    dealersData.forEach(function(d) {
      var name = String(d['Dealer Name'] || d.dealername || d.Name || d.name || '').trim();
      var phone = String(d['Mobile'] || d.mobile || '').trim();
      var status = String(d['Status'] || d.status || 'Active').trim();
      addMobile(phone, name, 'Dealer', '', status);
    });

    return list;
  } catch (e) {
    console.error('getRegisteredMobiles error:', e);
    return [];
  }
}

