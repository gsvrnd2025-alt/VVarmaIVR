// ============================================
// V4.1 UPGRADE — LOGO / SIGNATURE / SEAL CONFIG
// ============================================

function getOrCreateConfigSheet_(sheetName) {
  var ss = SpreadsheetApp.openById(CONFIG.spreadsheetId);
  var sheet = ss.getSheetByName(sheetName);
  if (!sheet) {
    sheet = ss.insertSheet(sheetName);
    sheet.appendRow(['Key', 'Value', 'Updated At']);
    sheet.getRange(1,1,1,3).setFontWeight('bold').setBackground('#f3f3f3');
  }
  return sheet;
}

function convertBase64ToDriveUrl_(base64Data, filename) {
  if (!base64Data || typeof base64Data !== 'string' || !base64Data.startsWith('data:image/')) return base64Data;
  try {
    var split = base64Data.split(',');
    var raw = split[1];
    var mimeType = split[0].match(/data:([^;]+);/)[1];
    var decoded = Utilities.base64Decode(raw);
    var blob = Utilities.newBlob(decoded, mimeType, filename);
    
    var folderName = "V-VARMA_Branding_Files";
    var folder, folders = DriveApp.getFoldersByName(folderName);
    if (folders.hasNext()) {
      folder = folders.next();
    } else {
      folder = DriveApp.createFolder(folderName);
    }
    
    // Trash existing branding file with exact name in this folder to prevent storage bloat
    var existing = folder.getFilesByName(filename);
    while (existing.hasNext()) {
      try {
        existing.next().setTrashed(true);
      } catch(de) {
        console.error("Error trashing file: " + de.message);
      }
    }
    
    var file = folder.createFile(blob);
    file.setSharing(DriveApp.Access.ANYONE_WITH_LINK, DriveApp.Permission.VIEW);
    return getDirectDriveUrl(file.getUrl());
  } catch(e) {
    console.error("Drive upload failed: " + e.message);
    return base64Data;
  }
}

function saveLogoConfig(data) {
  try {
    if (data.imageData) {
      data.imageData = convertBase64ToDriveUrl_(data.imageData, 'company_logo.png');
    }
    var sheet = getOrCreateConfigSheet_('Logo_Config');
    var existing = sheet.getDataRange().getValues();
    var keyMap = {};
    for (var i = 1; i < existing.length; i++) keyMap[String(existing[i][0])] = i + 1;
    Object.keys(data).forEach(function(key) {
      var ts = new Date().toISOString();
      if (keyMap[key]) { sheet.getRange(keyMap[key], 2).setValue(data[key]); sheet.getRange(keyMap[key], 3).setValue(ts); }
      else { sheet.appendRow([key, data[key], ts]); }
    });
    SpreadsheetApp.flush();
    return { success: true, imageUrl: data.imageData };
  } catch(e) { return { success: false, message: e.message }; }
}

function getLogoConfig() {
  try {
    var sheet = getOrCreateConfigSheet_('Logo_Config');
    var data = sheet.getDataRange().getValues();
    var config = { frameShape: 'circle', imageData: '', posX: 50, posY: 50, zoom: 100 };
    for (var i = 1; i < data.length; i++) { if (data[i][0]) config[String(data[i][0])] = data[i][1]; }
    return { success: true, config: config };
  } catch(e) { return { success: false, config: {}, message: e.message }; }
}

function saveSignatureConfig(data) {
  try {
    if (data.imageData) {
      data.imageData = convertBase64ToDriveUrl_(data.imageData, 'authorized_signature.png');
    }
    var sheet = getOrCreateConfigSheet_('Signature_Config');
    var existing = sheet.getDataRange().getValues();
    var keyMap = {};
    for (var i = 1; i < existing.length; i++) keyMap[String(existing[i][0])] = i + 1;
    Object.keys(data).forEach(function(key) {
      var ts = new Date().toISOString();
      if (keyMap[key]) { sheet.getRange(keyMap[key], 2).setValue(data[key]); sheet.getRange(keyMap[key], 3).setValue(ts); }
      else { sheet.appendRow([key, data[key], ts]); }
    });
    SpreadsheetApp.flush();
    return { success: true, imageUrl: data.imageData };
  } catch(e) { return { success: false, message: e.message }; }
}

function getSignatureConfig() {
  try {
    var sheet = getOrCreateConfigSheet_('Signature_Config');
    var data = sheet.getDataRange().getValues();
    var config = { imageData: '', width: 150, height: 60 };
    for (var i = 1; i < data.length; i++) { if (data[i][0]) config[String(data[i][0])] = data[i][1]; }
    return { success: true, config: config };
  } catch(e) { return { success: false, config: {}, message: e.message }; }
}

function saveSealConfig(data) {
  try {
    if (data.imageData) {
      data.imageData = convertBase64ToDriveUrl_(data.imageData, 'company_seal.png');
    }
    var sheet = getOrCreateConfigSheet_('Seal_Config');
    var existing = sheet.getDataRange().getValues();
    var keyMap = {};
    for (var i = 1; i < existing.length; i++) keyMap[String(existing[i][0])] = i + 1;
    Object.keys(data).forEach(function(key) {
      var ts = new Date().toISOString();
      if (keyMap[key]) { sheet.getRange(keyMap[key], 2).setValue(data[key]); sheet.getRange(keyMap[key], 3).setValue(ts); }
      else { sheet.appendRow([key, data[key], ts]); }
    });
    SpreadsheetApp.flush();
    return { success: true, imageUrl: data.imageData };
  } catch(e) { return { success: false, message: e.message }; }
}

function getSealConfig() {
  try {
    var sheet = getOrCreateConfigSheet_('Seal_Config');
    var data = sheet.getDataRange().getValues();
    var config = { imageData: '', size: 80, position: 'bottom-right', opacity: 0.5, rotation: 0 };
    for (var i = 1; i < data.length; i++) { if (data[i][0]) config[String(data[i][0])] = data[i][1]; }
    return { success: true, config: config };
  } catch(e) { return { success: false, config: {}, message: e.message }; }
}

// ============================================
// V4.1 UPGRADE — PUBLIC LINK SYSTEM
// ============================================

function generatePublicLink(docNumber) {
  try {
    var settingsRes = getSalesSettings();
    var settings = settingsRes.success ? settingsRes.settings : {};
    var expiryMinutes = parseInt(settings.PublicLinkExpiry) || 30;
    var singleUse = (settings.PublicLinkSingleUse === 'true');
    var ss = SpreadsheetApp.openById(CONFIG.spreadsheetId);
    var sheet = ss.getSheetByName('Public_Links');
    if (!sheet) {
      sheet = ss.insertSheet('Public_Links');
      sheet.appendRow(['Token','DocNumber','DocType','PartyName','Amount','Status','ExpiresAt','SingleUse','CreatedAt','PaidAt','TxnRef']);
      sheet.getRange(1,1,1,11).setFontWeight('bold').setBackground('#f3f3f3');
    }
    var billingData = getBillingHistory();
    var doc = billingData.find(function(d) { return String(d.DocNumber) === String(docNumber); });
    if (!doc) return { success: false, message: 'Document not found: ' + docNumber };
    var partyList = getPartyList();
    var party = partyList.find(function(p) { return p.Id === doc.PartyId; });
    var partyMobile = party ? (party.Mobile || '') : '';
    var partyEmail = party ? (party.Email || '') : '';

    var token = Utilities.getUuid();
    var now = new Date();
    var expiresAt = new Date(now.getTime() + expiryMinutes * 60000);
    sheet.appendRow([token, doc.DocNumber, doc.DocType, doc.PartyName, doc.GrandTotal, 'Active', expiresAt.toISOString(), singleUse ? 'true' : 'false', now.toISOString(), '', '']);
    SpreadsheetApp.flush();
    var appUrl = ScriptApp.getService().getUrl();
    var publicUrl = appUrl + '?page=invoice&token=' + token;
    return { success: true, token: token, url: publicUrl, expiresAt: expiresAt.toISOString(), partyMobile: partyMobile, partyEmail: partyEmail };
  } catch(e) { return { success: false, message: e.message }; }
}

function getPublicLinkData(token) {
  try {
    var ss = SpreadsheetApp.openById(CONFIG.spreadsheetId);
    var sheet = ss.getSheetByName('Public_Links');
    if (!sheet) return { success: false, message: 'Invalid link.', expired: true };
    var data = sheet.getDataRange().getValues();
    var h = data[0];
    var ti = h.indexOf('Token'), si = h.indexOf('Status'), ei = h.indexOf('ExpiresAt'), di = h.indexOf('DocNumber');
    for (var i = 1; i < data.length; i++) {
      if (String(data[i][ti]) !== String(token)) continue;
      var status = data[i][si];
      if (status !== 'Active') return { success: false, message: 'This payment link is ' + status + '.', expired: true };
      var expires = data[i][ei];
      if (expires && new Date(expires) < new Date()) {
        sheet.getRange(i+1, si+1).setValue('Expired');
        SpreadsheetApp.flush();
        return { success: false, message: 'This link has expired.', expired: true };
      }
      var docNumber = data[i][di];
      var billingData = getBillingHistory();
      var doc = billingData.find(function(d) { return String(d.DocNumber) === String(docNumber); });
      if (!doc) return { success: false, message: 'Invoice data not found.' };
      var settingsRes = getSalesSettings();
      var settings = settingsRes.success ? settingsRes.settings : {};
      var logoConf = getLogoConfig().config || {};
      var signConf = getSignatureConfig().config || {};
      var sealConf = getSealConfig().config || {};
      var partyList = getPartyList();
      var party = partyList.find(function(p) { return p.Id === doc.PartyId; });
      var items = [];
      try { items = JSON.parse(doc.ItemsJson || '[]'); } catch(e2) {}
      return {
        success: true, doc: doc, items: items,
        partyEmail: party ? (party.Email||'') : '',
        partyAddress: party ? (party.BillingAddress||party.Address||'') : '',
        partyMobile: party ? (party.Mobile||'') : '',
        partyGst: party ? (party.GstNumber||'') : '',
        settings: settings, logoConfig: logoConf, signatureConfig: signConf, sealConfig: sealConf,
        token: token, expiresAt: expires
      };
    }
    return { success: false, message: 'Invalid payment link.', expired: true };
  } catch(e) { return { success: false, message: e.message }; }
}

function getPublicLinkDataByMobileAndBill(mobile, bill) {
  try {
    if (!mobile || !bill) {
      return { success: false, message: 'Customer mobile number and bill number are required.' };
    }
    
    var cleanMobile = String(mobile).replace(/\D/g, '');
    if (!cleanMobile) {
      return { success: false, message: 'Invalid mobile number.' };
    }

    var billingData = getBillingHistory();
    var doc = billingData.find(function(d) { return String(d.DocNumber) === String(bill); });
    if (!doc) {
      return { success: false, message: 'Invoice not found: ' + bill };
    }

    var partyList = getPartyList();
    var party = partyList.find(function(p) { return p.Id === doc.PartyId; });
    if (!party) {
      return { success: false, message: 'Customer associated with this invoice was not found.' };
    }

    var m1 = cleanMobile;
    var m2 = String(party.Mobile || '').replace(/\D/g, '');
    var match = false;
    if (m1 && m2) {
      if (m1.length >= 10 && m2.length >= 10) {
        match = (m1.slice(-10) === m2.slice(-10));
      } else {
        match = (m1 === m2 || m1.indexOf(m2) !== -1 || m2.indexOf(m1) !== -1);
      }
    }

    if (!match) {
      return { success: false, message: 'Access denied: Customer mobile number does not match record.' };
    }

    var settingsRes = getSalesSettings();
    var settings = settingsRes.success ? settingsRes.settings : {};
    var logoConf = getLogoConfig().config || {};
    var signConf = getSignatureConfig().config || {};
    var sealConf = getSealConfig().config || {};
    var items = [];
    try { items = JSON.parse(doc.ItemsJson || '[]'); } catch(e2) {}

    var ss = SpreadsheetApp.openById(CONFIG.spreadsheetId);
    var sheet = ss.getSheetByName('Public_Links');
    var token = '';
    var expires = '';
    
    if (!sheet) {
      sheet = ss.insertSheet('Public_Links');
      sheet.appendRow(['Token','DocNumber','DocType','PartyName','Amount','Status','ExpiresAt','SingleUse','CreatedAt','PaidAt','TxnRef']);
      sheet.getRange(1,1,1,11).setFontWeight('bold').setBackground('#f3f3f3');
    }

    var data = sheet.getDataRange().getValues();
    var h = data[0];
    var ti = h.indexOf('Token'), di = h.indexOf('DocNumber'), si = h.indexOf('Status'), ei = h.indexOf('ExpiresAt');
    for (var i = 1; i < data.length; i++) {
      if (String(data[i][di]) === String(doc.DocNumber) && data[i][si] === 'Active') {
        token = data[i][ti];
        expires = data[i][ei];
        break;
      }
    }

    // If no active token found, generate one on the fly so deep links, QR codes, and confirmations work
    if (!token) {
      token = Utilities.getUuid();
      var now = new Date();
      var expiresAt = new Date(now.getTime() + 1440 * 60000); // 24 hours
      expires = expiresAt.toISOString();
      sheet.appendRow([token, doc.DocNumber, doc.DocType, doc.PartyName, doc.GrandTotal, 'Active', expires, 'false', now.toISOString(), '', '']);
      SpreadsheetApp.flush();
    }

    return {
      success: true, doc: doc, items: items,
      partyEmail: party ? (party.Email||'') : '',
      partyAddress: party ? (party.BillingAddress||party.Address||'') : '',
      partyMobile: party ? (party.Mobile||'') : '',
      partyGst: party ? (party.GstNumber||'') : '',
      settings: settings, logoConfig: logoConf, signatureConfig: signConf, sealConfig: sealConf,
      token: token, expiresAt: expires
    };
  } catch(e) {
    return { success: false, message: e.message };
  }
}

function markPublicLinkPaid(token, txnRef) {
  try {
    var ss = SpreadsheetApp.openById(CONFIG.spreadsheetId);
    var sheet = ss.getSheetByName('Public_Links');
    if (!sheet) return { success: false, message: 'Sheet not found' };
    var data = sheet.getDataRange().getValues();
    var h = data[0];
    var ti = h.indexOf('Token'), si = h.indexOf('Status'), pi = h.indexOf('PaidAt'), tri = h.indexOf('TxnRef');
    var dni = h.indexOf('DocNumber'), ami = h.indexOf('Amount'), pai = h.indexOf('PartyName');
    for (var i = 1; i < data.length; i++) {
      if (String(data[i][ti]) !== String(token)) continue;
      sheet.getRange(i+1, si+1).setValue('Paid');
      sheet.getRange(i+1, pi+1).setValue(new Date().toISOString());
      if (tri >= 0) sheet.getRange(i+1, tri+1).setValue(txnRef || '');
      SpreadsheetApp.flush();
      var docNumber = data[i][dni];
      try {
        var rSheet = getSheet('payment_receipts');
        rSheet.appendRow(['RCP-'+Date.now(),'',docNumber,new Date().toLocaleDateString('en-IN'),data[i][pai],'',data[i][ami],'UPI',txnRef||'',new Date().toLocaleDateString('en-IN'),'Auto via public link','[]','Received']);
        SpreadsheetApp.flush();
      } catch(pe) { console.error('Auto-receipt failed:', pe); }
      return { success: true, docNumber: docNumber };
    }
    return { success: false, message: 'Token not found' };
  } catch(e) { return { success: false, message: e.message }; }
}

function disablePublicLink(token) {
  try {
    var ss = SpreadsheetApp.openById(CONFIG.spreadsheetId);
    var sheet = ss.getSheetByName('Public_Links');
    if (!sheet) return { success: false };
    var data = sheet.getDataRange().getValues();
    var h = data[0];
    var ti = h.indexOf('Token'), si = h.indexOf('Status');
    for (var i = 1; i < data.length; i++) {
      if (String(data[i][ti]) === String(token)) {
        sheet.getRange(i+1, si+1).setValue('Disabled');
        SpreadsheetApp.flush();
        return { success: true };
      }
    }
    return { success: false, message: 'Token not found' };
  } catch(e) { return { success: false, message: e.message }; }
}

function getPublicLinksForDoc(docNumber) {
  try {
    var ss = SpreadsheetApp.openById(CONFIG.spreadsheetId);
    var sheet = ss.getSheetByName('Public_Links');
    if (!sheet) return { success: true, links: [] };
    var data = sheet.getDataRange().getValues();
    var headers = data[0];
    var links = [];
    for (var i = 1; i < data.length; i++) {
      var row = {};
      headers.forEach(function(hd, idx) { row[hd] = data[i][idx]; });
      if (String(row.DocNumber) === String(docNumber)) links.push(row);
    }
    return { success: true, links: links };
  } catch(e) { return { success: false, links: [], message: e.message }; }
}

// ============================================
// V4.1 UPGRADE — EMAIL AUTOMATION
// ============================================

function sendDocumentEmail(docNumber, recipientEmail, emailType) {
  try {
    if (!recipientEmail || !recipientEmail.includes('@')) return { success: false, message: 'Invalid email address' };
    var settingsRes = getSalesSettings();
    var settings = settingsRes.success ? settingsRes.settings : {};
    var compName = settings.CompanyName || 'V-VARMA ENTERPRISES';
    var pdfRes = downloadInvoicePDF(docNumber);
    if (!pdfRes.success) return { success: false, message: 'PDF generation failed: ' + pdfRes.message };
    var pdfBlob = Utilities.newBlob(Utilities.base64Decode(pdfRes.base64), 'application/pdf', pdfRes.filename);
    var subject, body;
    if (emailType === 'receipt') {
      subject = (settings.ReceiptEmailSubject || 'Payment Receipt - {docNumber} from {companyName}').replace('{docNumber}', docNumber).replace('{companyName}', compName);
      body = (settings.ReceiptEmailBody || 'Dear Customer,\n\nThank you for your payment. Please find your receipt attached.\n\nRegards,\n{companyName}').replace('{docNumber}', docNumber).replace('{companyName}', compName);
    } else {
      var linkRes = generatePublicLink(docNumber);
      var payLink = linkRes.success ? '\n\n\uD83D\uDD17 View & Pay Online: ' + linkRes.url : '';
      subject = (settings.InvoiceEmailSubject || 'Invoice {docNumber} from {companyName}').replace('{docNumber}', docNumber).replace('{companyName}', compName);
      body = (settings.InvoiceEmailBody || 'Dear Customer,\n\nPlease find your invoice {docNumber} attached.{payLink}\n\nRegards,\n{companyName}').replace('{docNumber}', docNumber).replace('{companyName}', compName).replace('{payLink}', payLink);
    }
    MailApp.sendEmail({ to: recipientEmail, subject: subject, body: body, attachments: [pdfBlob] });
    try {
      var logSheet = getOrCreateConfigSheet_('Email_Log');
      logSheet.appendRow(['EML-'+Date.now(), docNumber, recipientEmail, subject, 'Sent', new Date().toISOString(), '']);
      SpreadsheetApp.flush();
    } catch(le) {}
    return { success: true, message: 'Email sent to ' + recipientEmail };
  } catch(e) {
    try { getOrCreateConfigSheet_('Email_Log').appendRow(['EML-'+Date.now(), docNumber, recipientEmail, '', 'Failed', new Date().toISOString(), e.message]); SpreadsheetApp.flush(); } catch(le) {}
    return { success: false, message: 'Email failed: ' + e.message };
  }
}

// ============================================
// V4.1 UPGRADE — ENHANCED downloadInvoicePDF
// ============================================

function getDocTypeLabel_(docType) {
  var labels = {
    'Invoice':'TAX INVOICE','Tax Invoice':'TAX INVOICE','Proforma Invoice':'PROFORMA INVOICE',
    'Estimate':'ESTIMATE','Estimation':'ESTIMATE','Quotation':'QUOTATION',
    'Sales Order':'SALES ORDER','Delivery Challan':'DELIVERY CHALLAN',
    'Credit Note':'CREDIT NOTE','Debit Note':'DEBIT NOTE',
    'Receipt':'RECEIPT VOUCHER','Receipt Voucher':'RECEIPT VOUCHER'
  };
  return labels[docType] || (docType ? docType.toUpperCase() : 'INVOICE');
}

function isQrEnabledForDocType_(docType, settings) {
  var map = {
    // Standard names
    'Invoice':'QrInvoice','Tax Invoice':'QrInvoice','Sales Invoice':'QrSalesInvoice',
    'Proforma Invoice':'QrProformaInvoice','Quotation':'QrQuotation','Estimation':'QrEstimation',
    'Estimate':'QrEstimation','Credit Note':'QrCreditNote','Debit Note':'QrDebitNote',
    'Delivery Challan':'QrDeliveryChallan','Receipt':'QrReceipt',
    // Visual labels (Uppercase)
    'TAX INVOICE':'QrInvoice',
    'PROFORMA INVOICE':'QrProformaInvoice',
    'ESTIMATE':'QrEstimation',
    'QUOTATION':'QrQuotation',
    'CREDIT NOTE':'QrCreditNote',
    'DEBIT NOTE':'QrDebitNote',
    'DELIVERY CHALLAN':'QrDeliveryChallan',
    'RECEIPT VOUCHER':'QrReceipt'
  };
  var labelUpper = String(docType).toUpperCase().trim();
  
  // Conditional rule: QR on Dc and Estimate
  if ((labelUpper === 'ESTIMATE' || labelUpper === 'DELIVERY CHALLAN') && settings.EnableQrOnDcAndEstimate === 'false') {
    return false;
  }
  
  var key = map[docType] || map[labelUpper] || map[String(docType).trim()];
  return key ? (settings[key] === 'true') : false;
}



function formatDate_(dateStr) {
  if (!dateStr) return '';
  try {
    var d = new Date(dateStr);
    if (isNaN(d.getTime())) return dateStr;
    var day = ('0' + d.getDate()).slice(-2);
    var month = ('0' + (d.getMonth() + 1)).slice(-2);
    var year = d.getFullYear();
    return day + '-' + month + '-' + year;
  } catch(e) {
    return dateStr;
  }
}

function convertUrlToBase64_(url) {
  if (!url || typeof url !== 'string') return '';
  if (url.startsWith('data:image/')) return url;
  try {
    var isGoogleDrive = false;
    var fileId = '';
    
    if (url.includes('drive.google.com') || url.includes('docs.google.com') || url.includes('googleusercontent.com')) {
      var driveMatch = url.match(/\/file\/d\/([-\w]{25,})/);
      if (!driveMatch) driveMatch = url.match(/[?&]id=([-\w]{25,})/);
      if (!driveMatch) driveMatch = url.match(/\/d\/([-\w]{25,})/);
      if (driveMatch) {
        fileId = driveMatch[1];
        isGoogleDrive = true;
      }
    }
    
    if (isGoogleDrive && fileId) {
      var file = DriveApp.getFileById(fileId);
      var mime = file.getMimeType();
      var bytes = file.getBlob().getBytes();
      var base64 = Utilities.base64Encode(bytes);
      return 'data:' + mime + ';base64,' + base64;
    }
    
    var response = UrlFetchApp.fetch(url, { muteHttpExceptions: true });
    if (response.getResponseCode() === 200) {
      var blob = response.getBlob();
      var mime = blob.getMimeType();
      var bytes = blob.getBytes();
      var base64 = Utilities.base64Encode(bytes);
      return 'data:' + mime + ';base64,' + base64;
    }
  } catch(e) {
    console.error('convertUrlToBase64_ failed for ' + url + ': ' + e.message);
  }
  return url;
}

function downloadInvoicePDF(docNumber) {
  try {
    var billingData = getBillingHistory();
    var doc = billingData.find(function(d){ return String(d.DocNumber) === String(docNumber); });
    if (!doc) {
      var invoices = getData('invoices');
      var inv = invoices.find(function(i){ return String(i['Invoice Number']) === String(docNumber); });
      if (!inv) throw new Error('Document not found: ' + docNumber);
      doc = { DocType:'Invoice', DocNumber:inv['Invoice Number'], Date:inv['Invoice Date']||inv.Date,
        Subtotal:inv['Sub Total']||inv['Grand Total'], Discount:0, Tax:0, RoundOff:0,
        GrandTotal:inv['Grand Total'], AmountPaid:inv['Grand Total'], BalanceDue:0,
        PaymentMethod:inv['Payment Method']||'Cash', PaymentStatus:inv['Payment Status']||'Paid',
        DueDate:'', Notes:'Thank you!', GstEnabled:false, PartyId:'', PartyName:inv['Customer Name']||'' };
    }
    var settingsRes = getSalesSettings();
    var settings = settingsRes.success ? settingsRes.settings : {};
    
    // central logo/signature/seal config fetching
    var logoConf = getLogoConfig().config || {};
    var signConf = getSignatureConfig().config || {};
    var sealConf = getSealConfig().config || {};

    var compName = settings.CompanyName || 'V-VARMA ENTERPRISES';
    var compGst = settings.CompanyGst || '';
    var compAddress = settings.CompanyAddress || '';
    var compMobile = settings.CompanyMobile || '';
    var compEmail = settings.CompanyEmail || '';
    
    // Cross-fallback image resolver for Logo, Signature, and Seal
    var compLogo = logoConf.imageData || settings.CompanyLogo || '';
    var signImage = signConf.imageData || settings.SignatureImage || settings.CompanySignature || '';
    var sealImage = sealConf.imageData || settings.SealImage || settings.CompanySeal || '';
    
    var frameShape = logoConf.frameShape || 'circle';
    var sealPos = sealConf.position || 'bottom-right';
    var sealSize = parseInt(sealConf.size) || 80;
    var sealOpacity = parseFloat(sealConf.opacity) || 0.5;

    var marginTop = (parseInt(settings.MarginTop) || 10) + (parseInt(settings.ExtraTopSpace) || 0);
    var marginRight = parseInt(settings.MarginRight) || 8;
    var marginBottom = parseInt(settings.MarginBottom) || 10;
    var marginLeft = parseInt(settings.MarginLeft) || 8;

    var nsz = settings.NameTextSize;
    var companyNameSize = nsz === 'ExtraLarge' ? '38px' : nsz === 'Large' ? '32px' : nsz === 'Medium' ? '28px' : '24px';
    var itsz = settings.InvoiceTextSize;
    var docTypeSize = itsz === 'ExtraLarge' ? '28px' : itsz === 'Large' ? '24px' : itsz === 'Medium' ? '20px' : '17px';
    var regTheme = settings.RegularTheme || 'Theme 1';
    if (regTheme === 'Theme 1') regTheme = 'Classic M1';
    if (regTheme === 'Theme 2') regTheme = 'Premium Model Example';
    var primaryColor = settings.RegularPrimaryColor || '#1a365d';
    
    // Provide standard defaults for themes if missing/black
    if (!primaryColor || primaryColor === '#000000') {
      if (regTheme.indexOf('Classic') !== -1) primaryColor = '#92c038';
      if (regTheme === 'Premium Model Example') primaryColor = '#d92332';
    }

    var subTotalVal = parseFloat(doc.Subtotal || 0);
    var discountVal = parseFloat(doc.Discount || 0);
    var gstVal = parseFloat(doc.Tax || 0);
    var roundOffVal = parseFloat(doc.RoundOff || 0);
    var grandTotalVal = parseFloat(doc.GrandTotal || 0);
    var paidVal = parseFloat(doc.AmountPaid || doc.GrandTotal || 0);
    var balanceVal = parseFloat(doc.BalanceDue || 0);

    var gstEnabled = (doc.GstEnabled === true || String(doc.GstEnabled).toLowerCase() === 'true');
    var docType = doc.DocType || 'Invoice';
    var docTypeLabel = getDocTypeLabel_(docType);
    if ((docType === 'Invoice' || docType === 'Tax Invoice' || docType === 'Sales Invoice') && !gstEnabled) {
      docTypeLabel = 'PROFORMA INVOICE';
    }

    var upiId = settings.CompanyUpi || '';
    var payeeName = settings.CompanyUpiName || compName;
    var payAmt = parseFloat(doc.GrandTotal) || 0;

    var logoBorderRadius = frameShape === 'circle' ? '50%' : frameShape === 'rounded' ? '10px' : frameShape === 'horizontal-oval' ? '50%/30%' : frameShape === 'vertical-oval' ? '30%/50%' : '0';
    
    // Convert logo, signature, and seal to Base64 for flawless PDF rendering
    var compLogoBase64 = convertUrlToBase64_(compLogo);
    var signImageBase64 = convertUrlToBase64_(signImage);
    var sealImageBase64 = convertUrlToBase64_(sealImage);

    // ===================================
    // DYNAMIC SETTINGS VISIBILITY CONTROLS
    // ===================================
    var useDecimals = (settings.AmountWithDecimals !== 'false');
    function formatAmt_(val) {
      var num = parseFloat(val || 0);
      return useDecimals ? num.toFixed(2) : String(Math.round(num));
    }

    // ===================================
    // DYNAMIC SETTINGS VISIBILITY CONTROLS
    // ===================================
    var showLogo = (settings.RegularShowLogo !== 'false');
    var showSignatory = (settings.RegularShowSignatory !== 'false');
    var showQR = (settings.RegularShowQR !== 'false') && isQrEnabledForDocType_(docTypeLabel, settings);
    var showBank = (settings.RegularShowBank !== 'false');
    var showTC = (settings.RegularShowTC !== 'false');
    var showAck = (settings.RegularShowAck === 'true');
    var showCheckboxes = (settings.PrintOriginalDuplicate !== 'false');

    var logoHTML = '';
    if (showLogo) {
      if (compLogoBase64) {
        logoHTML = '<img src="' + compLogoBase64 + '" style="max-height: 55px; max-width: 140px; object-fit: contain; border-radius: ' + logoBorderRadius + ';">';
      } else {
        logoHTML = '<div style="font-weight: 800; font-size: 32px; color: ' + primaryColor + ';">G⚡V</div>';
      }
    }

    var sigDisplayWidth = parseInt(signConf.width) || 150;
    var sigDisplayHeight = parseInt(signConf.height) || 60;
    var sealRotation = parseInt(sealConf.rotation) || 0;

    var signatoryHTML = '';
    if (showSignatory) {
      var sigImg = signImageBase64 ? '<img src="' + signImageBase64 + '" style="max-width:' + sigDisplayWidth + 'px;max-height:' + sigDisplayHeight + 'px;object-fit:contain;margin-bottom:4px;">' : '<div style="height:50px;"></div>';
      var sealImgHtml = sealImageBase64 ? '<img src="' + sealImageBase64 + '" style="width:' + sealSize + 'px;height:' + sealSize + 'px;object-fit:contain;opacity:' + sealOpacity + ';transform:rotate(' + sealRotation + 'deg);">' : '';
      signatoryHTML = 
        '<table style="width: 100%; border-collapse: collapse; border: none; margin-top: 15px;" border="0">' +
          '<tr>' +
            '<td style="width: 40%; text-align: center; vertical-align: middle; border: none; padding-right: 10px;">' +
              (sealImageBase64 ? sealImgHtml : '') +
            '</td>' +
            '<td style="width: 60%; text-align: center; vertical-align: top; border: none;">' +
              '<div style="text-align: center; font-family:\'Segoe UI\', Arial, sans-serif; display: inline-block; min-width: 150px;">' +
                '<div style="margin-bottom: 5px; font-size: 11px; font-weight: 500;">For ' + compName + '</div>' +
                sigImg +
                '<div style="border-top:1px solid #333;padding-top:4px;font-weight:bold;font-size:11px;">' + (settings.FooterAuthorizedSignatory || 'Authorized Signatory') + '</div>' +
              '</div>' +
            '</td>' +
          '</tr>' +
        '</table>';
    }

    var qrImgHtml = '';
    if (showQR && upiId && payAmt > 0) {
      var upiStr = 'upi://pay?pa=' + encodeURIComponent(upiId) + '&pn=' + encodeURIComponent(payeeName) + '&am=' + payAmt.toFixed(2) + '&cu=INR&tn=' + encodeURIComponent(docTypeLabel + ' ' + doc.DocNumber);
      var qrApi = 'https://api.qrserver.com/v1/create-qr-code/?size=150x150&data=' + encodeURIComponent(upiStr);
      var qrApiBase64 = convertUrlToBase64_(qrApi);
      
      // Double-fallback retry to Google Charts API if qrserver base64 is missing or fails
      if (!qrApiBase64 || qrApiBase64 === qrApi) {
        console.warn('convertUrlToBase64_ failed for qrserver, falling back to Google Charts API');
        var qrFallback = 'https://chart.googleapis.com/chart?chs=150x150&cht=qr&chl=' + encodeURIComponent(upiStr);
        qrApiBase64 = convertUrlToBase64_(qrFallback);
      }
      
      qrImgHtml = 
        '<div style="text-align: center; border: 1px solid #ccc; border-radius: 4px; padding: 6px; background:#fff; width: 110px; display:inline-block; font-family:\'Segoe UI\', Arial, sans-serif;">' +
          '<div style="font-size: 8px; font-weight: bold; margin-bottom: 4px; color: ' + primaryColor + ';">SCAN & PAY VIA UPI</div>' +
          '<img src="' + qrApiBase64 + '" style="width: 90px; height: 90px; display:block; margin: 0 auto;" />' +
          '<div style="background:#25d366; color: white; font-size:8px; font-weight:bold; padding:2px; display:block; margin-top:4px; border-radius:2px;">UPI SCAN TO PAY</div>' +
        '</div>';
    }

    var checkBoxesHtml = '';
    if (showCheckboxes) {
      checkBoxesHtml = 
        '<table style="width: 100%; border-collapse: collapse; margin-bottom: 5px; border: none;" border="0">' +
          '<tr>' +
            '<td style="text-align: right; font-size: 9px; font-family: \'Segoe UI\', Arial, sans-serif; border: none; color:#555;">' +
              'Original for Recipient <span style="width:10px; height:10px; border:1px solid #333; display:inline-block; margin-left:3px; vertical-align:middle;"></span> &nbsp;&nbsp;&nbsp;' +
              'Duplicate for Transporter <span style="width:10px; height:10px; border:1px solid #333; display:inline-block; margin-left:3px; vertical-align:middle;"></span> &nbsp;&nbsp;&nbsp;' +
              'Triplicate for Supplier <span style="width:10px; height:10px; border:1px solid #333; display:inline-block; margin-left:3px; vertical-align:middle;"></span>' +
            '</td>' +
          '</tr>' +
        '</table>';
    }

    var items = [];
    try { items = JSON.parse(doc.ItemsJson || '[]'); } catch(e2) { items = []; }
    var partyList = getPartyList();
    var party = partyList.find(function(p){ return p.Id === doc.PartyId; });
    var partyName = doc.PartyName || (party ? party.Name : 'Cash Customer');
    var partyMobile = party ? (party.Mobile||'') : '';
    var partyGst = party ? (party.GstNumber||'') : '';
    var partyAddress = party ? (party.BillingAddress||party.Address||'') : '';
    var partyEmail = party ? (party.Email||'') : '';

    var totalQty = 0;
    var totalTaxable = 0, totalSgst = 0, totalCgst = 0;
    var tableHeaderHtml = '';
    if (gstEnabled) {
      tableHeaderHtml = 
        '<tr style="background-color: ' + (regTheme === 'Classic M3' ? 'transparent' : primaryColor) + '; color: ' + (regTheme === 'Classic M3' ? '#000' : 'white') + '; text-align: left; font-weight: bold;">' +
          '<th style="padding: 6px; border: 1px solid #ccc; width: 3%;">#</th>' +
          '<th style="padding: 6px; border: 1px solid #ccc; width: 14%;">Item name</th>' +
          '<th style="padding: 6px; border: 1px solid #ccc; width: 10%;">Model No/Name</th>' +
          '<th style="padding: 6px; border: 1px solid #ccc; width: 10%;">Serial No</th>' +
          '<th style="padding: 6px; border: 1px solid #ccc; width: 5%;">HSN</th>' +
          '<th style="padding: 6px; border: 1px solid #ccc; width: 5%; text-align: right;">Qty</th>' +
          '<th style="padding: 6px; border: 1px solid #ccc; width: 5%; text-align: right;">Unit</th>' +
          '<th style="padding: 6px; border: 1px solid #ccc; width: 8%; text-align: right;">Rate</th>' +
          '<th style="padding: 6px; border: 1px solid #ccc; width: 4%; text-align: right;">Disc%</th>' +
          '<th style="padding: 6px; border: 1px solid #ccc; width: 8%; text-align: right;">Taxable</th>' +
          '<th style="padding: 6px; border: 1px solid #ccc; width: 4%; text-align: right;">SGST%</th>' +
          '<th style="padding: 6px; border: 1px solid #ccc; width: 6%; text-align: right;">SGST</th>' +
          '<th style="padding: 6px; border: 1px solid #ccc; width: 4%; text-align: right;">CGST%</th>' +
          '<th style="padding: 6px; border: 1px solid #ccc; width: 6%; text-align: right;">CGST</th>' +
          '<th style="padding: 6px; border: 1px solid #ccc; width: 8%; text-align: right;">Amount</th>' +
        '</tr>';
    } else {
      tableHeaderHtml = 
        '<tr style="background-color: ' + (regTheme === 'Classic M3' ? 'transparent' : primaryColor) + '; color: ' + (regTheme === 'Classic M3' ? '#000' : 'white') + '; text-align: left; font-weight: bold;">' +
          '<th style="padding: 6px; border: 1px solid #ccc; width: 5%;">#</th>' +
          '<th style="padding: 6px; border: 1px solid #ccc; width: 25%;">Item name</th>' +
          '<th style="padding: 6px; border: 1px solid #ccc; width: 15%;">Model No/Name</th>' +
          '<th style="padding: 6px; border: 1px solid #ccc; width: 15%;">Serial No</th>' +
          '<th style="padding: 6px; border: 1px solid #ccc; width: 10%; text-align: right;">Quantity</th>' +
          '<th style="padding: 6px; border: 1px solid #ccc; width: 10%; text-align: right;">Unit</th>' +
          '<th style="padding: 6px; border: 1px solid #ccc; width: 10%; text-align: right;">Price/ Unit</th>' +
          '<th style="padding: 6px; border: 1px solid #ccc; width: 10%; text-align: right;">Amount</th>' +
        '</tr>';
    }

    var showProductDesc = (settings.PrintProductDescriptions !== 'false');
    var showTotalItems = (settings.ShowTotalItems !== 'false');
    var minRows = parseInt(settings.MinRowsInItemsTable) || 0;

    var itemRowsHtml = '';
    items.forEach(function(item, idx) {
      if (!item.name && !item.ProductName) return;
      var nm = item.name || item.ProductName || 'Item';
      var desc = (item.desc && showProductDesc) ? '<br><span style="font-size:8px;color:#666;font-style:italic;">' + item.desc + '</span>' : '';
      var modelNo = item.modelNo || item.modelNumber || '';
      var modelName = item.modelName || '';
      var hsn = item.hsn || item.hsnCode || item.sacCode || '';
      var serials = item.serialNumbers || (item.sn ? [item.sn] : []);
      var qty = parseFloat(item.qty || 1);
      totalQty += qty;
      var unit = item.unit || 'Nos';
      var rate = parseFloat(item.rate || 0);
      var discPct = parseFloat(item.disc || 0);
      var gstPct = parseFloat(item.gst || 0);
      var discAmt = (qty * rate * discPct) / 100;
      var taxable = (qty * rate) - discAmt;
      var halfGst = gstPct / 2;
      var sgst = gstEnabled ? (taxable * halfGst / 100) : 0;
      var cgst = gstEnabled ? (taxable * halfGst / 100) : 0;
      var total = parseFloat(item.total || (taxable + sgst + cgst));
      totalTaxable += taxable; totalSgst += sgst; totalCgst += cgst;

      var modelCellVal = modelNo ? modelNo + (modelName ? ' / ' + modelName : '') : '-';
      var serialsCellVal = serials.length > 0 ? serials.join(', ') : '-';

      if (gstEnabled) {
        itemRowsHtml += '<tr>' +
          '<td style="padding: 5px; border: 1px solid #ccc;">' + (idx+1) + '</td>' +
          '<td style="padding: 5px; border: 1px solid #ccc; font-weight: bold;">' + nm + desc + '</td>' +
          '<td style="padding: 5px; border: 1px solid #ccc; font-size: 9px;">' + modelCellVal + '</td>' +
          '<td style="padding: 5px; border: 1px solid #ccc; font-size: 9px;">' + serialsCellVal + '</td>' +
          '<td style="padding: 5px; border: 1px solid #ccc;">' + (hsn||'-') + '</td>' +
          '<td style="padding: 5px; border: 1px solid #ccc; text-align: right;">' + qty + '</td>' +
          '<td style="padding: 5px; border: 1px solid #ccc; text-align: right;">' + unit + '</td>' +
          '<td style="padding: 5px; border: 1px solid #ccc; text-align: right;">₹' + formatAmt_(rate) + '</td>' +
          '<td style="padding: 5px; border: 1px solid #ccc; text-align: right;">' + (discPct > 0 ? discPct+'%' : '-') + '</td>' +
          '<td style="padding: 5px; border: 1px solid #ccc; text-align: right;">₹' + formatAmt_(taxable) + '</td>' +
          '<td style="padding: 5px; border: 1px solid #ccc; text-align: right;">' + halfGst + '%</td>' +
          '<td style="padding: 5px; border: 1px solid #ccc; text-align: right;">₹' + formatAmt_(sgst) + '</td>' +
          '<td style="padding: 5px; border: 1px solid #ccc; text-align: right;">' + halfGst + '%</td>' +
          '<td style="padding: 5px; border: 1px solid #ccc; text-align: right;">₹' + formatAmt_(cgst) + '</td>' +
          '<td style="padding: 5px; border: 1px solid #ccc; text-align: right; font-weight: bold;">₹' + formatAmt_(total) + '</td>' +
        '</tr>';
      } else {
        itemRowsHtml += '<tr>' +
          '<td style="padding: 5px; border: 1px solid #ccc;">' + (idx+1) + '</td>' +
          '<td style="padding: 5px; border: 1px solid #ccc; font-weight: bold;">' + nm + desc + '</td>' +
          '<td style="padding: 5px; border: 1px solid #ccc; font-size: 9px;">' + modelCellVal + '</td>' +
          '<td style="padding: 5px; border: 1px solid #ccc; font-size: 9px;">' + serialsCellVal + '</td>' +
          '<td style="padding: 5px; border: 1px solid #ccc; text-align: right;">' + qty + '</td>' +
          '<td style="padding: 5px; border: 1px solid #ccc; text-align: right;">' + unit + '</td>' +
          '<td style="padding: 5px; border: 1px solid #ccc; text-align: right;">₹' + formatAmt_(rate) + '</td>' +
          '<td style="padding: 5px; border: 1px solid #ccc; text-align: right; font-weight: bold;">₹' + formatAmt_(total) + '</td>' +
        '</tr>';
      }
    });

    // Min Rows in Items Table Padding Logic
    var actualRowsCount = items.filter(function(item) { return item.name || item.ProductName; }).length;
    var paddingRowsNeeded = minRows - actualRowsCount;
    if (paddingRowsNeeded > 0) {
      for (var p = 0; p < paddingRowsNeeded; p++) {
        if (gstEnabled) {
          itemRowsHtml += '<tr>' +
            '<td style="padding: 5px; border: 1px solid #ccc; color: transparent;">' + (actualRowsCount + p + 1) + '</td>' +
            '<td style="padding: 5px; border: 1px solid #ccc; color: transparent;">&nbsp;</td>' +
            '<td style="padding: 5px; border: 1px solid #ccc; color: transparent;">&nbsp;</td>' +
            '<td style="padding: 5px; border: 1px solid #ccc; color: transparent;">&nbsp;</td>' +
            '<td style="padding: 5px; border: 1px solid #ccc; color: transparent;">&nbsp;</td>' +
            '<td style="padding: 5px; border: 1px solid #ccc; color: transparent;">&nbsp;</td>' +
            '<td style="padding: 5px; border: 1px solid #ccc; color: transparent;">&nbsp;</td>' +
            '<td style="padding: 5px; border: 1px solid #ccc; color: transparent;">&nbsp;</td>' +
            '<td style="padding: 5px; border: 1px solid #ccc; color: transparent;">&nbsp;</td>' +
            '<td style="padding: 5px; border: 1px solid #ccc; color: transparent;">&nbsp;</td>' +
            '<td style="padding: 5px; border: 1px solid #ccc; color: transparent;">&nbsp;</td>' +
            '<td style="padding: 5px; border: 1px solid #ccc; color: transparent;">&nbsp;</td>' +
            '<td style="padding: 5px; border: 1px solid #ccc; color: transparent;">&nbsp;</td>' +
            '<td style="padding: 5px; border: 1px solid #ccc; color: transparent;">&nbsp;</td>' +
            '<td style="padding: 5px; border: 1px solid #ccc; color: transparent; font-weight: bold;">&nbsp;</td>' +
          '</tr>';
        } else {
          itemRowsHtml += '<tr>' +
            '<td style="padding: 5px; border: 1px solid #ccc; color: transparent;">' + (actualRowsCount + p + 1) + '</td>' +
            '<td style="padding: 5px; border: 1px solid #ccc; color: transparent;">&nbsp;</td>' +
            '<td style="padding: 5px; border: 1px solid #ccc; color: transparent;">&nbsp;</td>' +
            '<td style="padding: 5px; border: 1px solid #ccc; color: transparent;">&nbsp;</td>' +
            '<td style="padding: 5px; border: 1px solid #ccc; color: transparent;">&nbsp;</td>' +
            '<td style="padding: 5px; border: 1px solid #ccc; color: transparent;">&nbsp;</td>' +
            '<td style="padding: 5px; border: 1px solid #ccc; color: transparent;">&nbsp;</td>' +
            '<td style="padding: 5px; border: 1px solid #ccc; color: transparent; font-weight: bold;">&nbsp;</td>' +
          '</tr>';
        }
      }
    }

    var totalRowHtml = '';
    if (gstEnabled) {
      totalRowHtml = '<tr style="font-weight: bold;' + (regTheme === 'Classic M3' ? '' : 'background-color:' + primaryColor + ';color:white;') + '">' +
        '<td style="padding: 6px; border: 1px solid #ccc;"></td>' +
        '<td style="padding: 6px; border: 1px solid #ccc;">Total</td>' +
        '<td style="padding: 6px; border: 1px solid #ccc;"></td>' +
        '<td style="padding: 6px; border: 1px solid #ccc;"></td>' +
        '<td style="padding: 6px; border: 1px solid #ccc;"></td>' +
        '<td style="padding: 6px; border: 1px solid #ccc; text-align: right;">' + (showTotalItems ? totalQty : '') + '</td>' +
        '<td style="padding: 6px; border: 1px solid #ccc;"></td>' +
        '<td style="padding: 6px; border: 1px solid #ccc;"></td>' +
        '<td style="padding: 6px; border: 1px solid #ccc;"></td>' +
        '<td style="padding: 6px; border: 1px solid #ccc; text-align: right;">₹' + formatAmt_(totalTaxable) + '</td>' +
        '<td style="padding: 6px; border: 1px solid #ccc;"></td>' +
        '<td style="padding: 6px; border: 1px solid #ccc; text-align: right;">₹' + formatAmt_(totalSgst) + '</td>' +
        '<td style="padding: 6px; border: 1px solid #ccc;"></td>' +
        '<td style="padding: 6px; border: 1px solid #ccc; text-align: right;">₹' + formatAmt_(totalCgst) + '</td>' +
        '<td style="padding: 6px; border: 1px solid #ccc; text-align: right;">₹' + formatAmt_(grandTotalVal) + '</td>' +
      '</tr>';
    } else {
      totalRowHtml = '<tr style="font-weight: bold;' + (regTheme === 'Classic M3' ? '' : 'background-color:' + primaryColor + ';color:white;') + '">' +
        '<td style="padding: 6px; border: 1px solid #ccc;"></td>' +
        '<td style="padding: 6px; border: 1px solid #ccc;">Total</td>' +
        '<td style="padding: 6px; border: 1px solid #ccc;"></td>' +
        '<td style="padding: 6px; border: 1px solid #ccc;"></td>' +
        '<td style="padding: 6px; border: 1px solid #ccc; text-align: right;">' + (showTotalItems ? totalQty : '') + '</td>' +
        '<td style="padding: 6px; border: 1px solid #ccc;"></td>' +
        '<td style="padding: 6px; border: 1px solid #ccc;"></td>' +
        '<td style="padding: 6px; border: 1px solid #ccc; text-align: right;">₹' + formatAmt_(grandTotalVal) + '</td>' +
      '</tr>';
    }

    function getTotalsTableHtml(themeName) {
      var rowsHtml = '';
      rowsHtml += '<tr><td style="padding: 6px; border-bottom: 1px solid #ccc;">Sub Total</td><td style="padding: 6px; text-align: right; border-bottom: 1px solid #ccc;">₹' + formatAmt_(subTotalVal) + '</td></tr>';
      
      if (discountVal > 0) {
        rowsHtml += '<tr style="color:#38A169;"><td style="padding: 6px; border-bottom: 1px solid #ccc;">Discount</td><td style="padding: 6px; text-align: right; border-bottom: 1px solid #ccc;">-₹' + formatAmt_(discountVal) + '</td></tr>';
      }
      
      if (gstEnabled) {
        var halfGstVal = gstVal / 2;
        if (halfGstVal > 0) {
          rowsHtml += '<tr style="color:#3182CE;"><td style="padding: 6px; border-bottom: 1px solid #ccc;">CGST</td><td style="padding: 6px; text-align: right; border-bottom: 1px solid #ccc;">₹' + formatAmt_(halfGstVal) + '</td></tr>';
          rowsHtml += '<tr style="color:#3182CE;"><td style="padding: 6px; border-bottom: 1px solid #ccc;">SGST</td><td style="padding: 6px; text-align: right; border-bottom: 1px solid #ccc;">₹' + formatAmt_(halfGstVal) + '</td></tr>';
        }
      }
      
      if (roundOffVal !== 0) {
        rowsHtml += '<tr style="color:#718096;"><td style="padding: 6px; border-bottom: 1px solid #ccc;">Round Off</td><td style="padding: 6px; text-align: right; border-bottom: 1px solid #ccc;">₹' + formatAmt_(roundOffVal) + '</td></tr>';
      }
      
      var totalLabelStyle = '';
      if (themeName === 'Classic M2') {
        totalLabelStyle = 'background: ' + primaryColor + '; color: white;';
      } else if (themeName === 'Modern M1') {
        totalLabelStyle = 'background: ' + primaryColor + '; color: white; font-weight: bold;';
      } else if (themeName === 'Premium Model Example') {
        totalLabelStyle = 'background-color: ' + primaryColor + '; color: white; font-weight: bold;';
      } else {
        totalLabelStyle = 'font-weight: bold; border-bottom: 1px solid #ccc;';
      }
      
      rowsHtml += '<tr style="' + totalLabelStyle + '"><td style="padding: 6px;">Total</td><td style="padding: 6px; text-align: right;">₹' + formatAmt_(grandTotalVal) + '</td></tr>';
      
      var showReceived = (settings.ShowReceivedAmount !== 'false');
      var showBalance = (settings.ShowBalanceDueAmount !== 'false');

      if (showReceived) {
        rowsHtml += '<tr><td style="padding: 6px; border-bottom: 1px solid #ccc;">Received</td><td style="padding: 6px; text-align: right; border-bottom: 1px solid #ccc;">₹' + formatAmt_(paidVal) + '</td></tr>';
      }
      
      if (showBalance && balanceVal > 0) {
        rowsHtml += '<tr style="color:#E53E3E; font-weight:bold;"><td style="padding: 6px; border-bottom: 1px solid #ccc;">Balance</td><td style="padding: 6px; text-align: right; border-bottom: 1px solid #ccc;">₹' + formatAmt_(balanceVal) + '</td></tr>';
      }

      if (settings.ShowPartyCurrentBalance !== 'false' && party) {
        var currentOutstanding = parseFloat(party.CurrentBalance || 0);
        var prevBalance = currentOutstanding - balanceVal;
        rowsHtml += '<tr style="color:#718096; border-top:1px solid #ccc; font-size:10px;"><td style="padding: 6px;">Previous Balance:</td><td style="padding: 6px; text-align: right;">₹' + formatAmt_(prevBalance) + '</td></tr>';
        rowsHtml += '<tr style="color:#2D3748; border-top:1px solid #ccc; font-size:10px; font-weight:bold;"><td style="padding: 6px;">Current Balance:</td><td style="padding: 6px; text-align: right;">₹' + formatAmt_(currentOutstanding) + '</td></tr>';
      }
      
      return '<table style="width: 100%; border-collapse: collapse; font-size: 11px;">' + rowsHtml + '</table>';
    }

    var amtInWords = convertNumberToWords(grandTotalVal);

    var showSavedDiscount = (settings.PrintSavedDiscount === 'true') && discountVal > 0;
    var savedDiscountHtml = '';
    if (showSavedDiscount) {
      savedDiscountHtml = '<div style="margin-top: 10px; padding: 6px; background-color: #e6fffa; color: #319795; font-weight: bold; border-radius: 4px; display: inline-block; font-size: 11px; border: 1px solid #b2f5ea;">🎉 You Saved ₹' + formatAmt_(discountVal) + '!</div>';
    }

    var tcText = settings.FooterTC || doc.Notes || 'Goods once sold will not be taken back.';

    // ===================================
    // DYNAMIC BANK DETAILS RENDERING
    // ===================================
    var bankDetailsHtml = '';
    if (showBank && settings.BankName) {
      bankDetailsHtml = 
        '<b>Bank Name :</b> ' + settings.BankName + '<br>' +
        '<b>Bank Account No. :</b> ' + settings.BankAccNo + '<br>' +
        '<b>Bank IFSC code :</b> ' + settings.BankIfsc + '<br>' +
        (settings.BankBranch ? '<b>Branch :</b> ' + settings.BankBranch + '<br>' : '') +
        '<b>Account holder\'s name :</b> ' + (settings.BankAccHolder || compName);
    }

    var tcBlockHtml = '';
    if (showTC) {
      tcBlockHtml = 
        '<div style="background-color: ' + primaryColor + '; color: white; padding: 4px 8px; font-weight: bold; font-size: 10px; border-radius: 2px;">Terms and Conditions</div>' +
        '<div style="padding: 6px; font-size: 9px; line-height: 1.4; color: #2d3748; border: 1px solid #ccc; border-top: none; background: #fdfdfd; min-height: 50px;">' + tcText.replace(/\n/g, '<br>') + '</div>';
    }

    var bankBlockHtml = '';
    if (showBank && settings.BankName) {
      bankBlockHtml = 
        '<div style="background-color: ' + primaryColor + '; color: white; padding: 4px 8px; font-weight: bold; font-size: 10px; border-radius: 2px; margin-top: 10px;">Bank details:</div>' +
        '<div style="padding: 6px; font-size: 9px; line-height: 1.4; color: #2d3748; border: 1px solid #ccc; border-top: none; background: #fdfdfd;">' + bankDetailsHtml + '</div>';
    }

    // Dynamic 3-Column Footer Assembler based on Enabled Logics
    var footerColumns = [];
    
    // Left column: Terms and Bank (taking remaining or 45% space)
    if (showTC || (showBank && settings.BankName)) {
      var leftContent = '';
      if (showTC) leftContent += tcBlockHtml;
      if (showBank && settings.BankName) leftContent += bankBlockHtml;
      footerColumns.push({
        width: showQR ? '45%' : '60%',
        content: leftContent
      });
    }
    
    // Middle column: UPI QR Code
    if (showQR && upiId && payAmt > 0) {
      footerColumns.push({
        width: '20%',
        content: '<div style="text-align: center;">' + qrImgHtml + '</div>'
      });
    }
    
    // Right column: Signatory
    if (showSignatory) {
      footerColumns.push({
        width: '35%',
        content: signatoryHTML
      });
    }
    
    // Compile dynamic footer table rows
    var footerCellsHtml = '';
    footerColumns.forEach(function(col, i) {
      footerCellsHtml += '<td style="width: ' + col.width + '; vertical-align: top; border: none; padding-right: ' + (i < footerColumns.length - 1 ? '15px' : '0') + ';">' + col.content + '</td>';
    });
    
    var dynamicFooterTableHtml = 
      '<table style="width: 100%; border-collapse: collapse; border: none; margin-top: 25px;" border="0">' +
        '<tr>' + footerCellsHtml + '</tr>' +
      '</table>';

    var showAck = (settings.RegularShowAck === 'true');
    var ackHtml = '';
    if (showAck) {
      var formattedDocDate = formatDate_(doc.Date || doc.InvoiceDate || '');
      ackHtml = getAcknowledgementPageHtml_({
        regTheme: regTheme,
        primaryColor: primaryColor,
        compName: compName,
        compAddress: compAddress,
        compGst: compGst,
        compMobile: compMobile,
        compEmail: compEmail,
        compLogoBase64: compLogoBase64,
        logoBorderRadius: logoBorderRadius,
        logoHTML: logoHTML,
        companyNameSize: companyNameSize,
        docTypeSize: docTypeSize,
        docTypeLabel: docTypeLabel,
        doc: doc,
        partyName: partyName,
        partyAddress: partyAddress,
        partyMobile: partyMobile,
        formattedDocDate: formattedDocDate,
        grandTotalVal: grandTotalVal,
        settings: settings,
        checkBoxesHtml: checkBoxesHtml
      });
    }

    var formattedDate = formatDate_(doc.Date || doc.InvoiceDate || '');

    var billToHtml = 
      '<div style="font-weight: bold; font-size: 14px; color: #1a202c; margin-bottom: 4px;">' + partyName + '</div>' +
      '<div style="font-size: 11px; line-height: 1.5; color: #2d3748; font-weight: 500;">' +
        (partyMobile ? '<b>Ph:</b> ' + partyMobile + '<br>' : '') +
        (partyEmail ? '<b>Email:</b> ' + partyEmail + '<br>' : '') +
        (gstEnabled && partyGst ? '<b>GSTIN:</b> ' + partyGst + '<br>' : '') +
        '<b>Address:</b> ' + (partyAddress || '-') +
      '</div>';

    var invoiceDetailsHtml = 
      '<div><b>' + docTypeLabel + ' No. :</b> ' + doc.DocNumber + '</div>' +
      '<div style="margin-top: 5px;"><b>Date :</b> ' + formattedDate + '</div>' +
      (doc.DueDate ? '<div style="margin-top: 5px; color:#E53E3E;"><b>Due Date :</b> ' + doc.DueDate + '</div>' : '');

    var contentHtml = '';

    // ===================================
    // THEME 1: CLASSIC M1
    // ===================================
    if (regTheme === 'Classic M1') {
      contentHtml = 
        '<div style="font-family: \'Segoe UI\', Arial, sans-serif; color: #000; margin: 0; padding: 0; background: #FFF; box-sizing: border-box; position: relative;">' +
          '<table style="width: 100%; border-collapse: collapse; background-color: ' + primaryColor + '; color: white; border: none; margin-bottom: 10px;" cellpadding="15" border="0">' +
            '<tr>' +
              '<td style="width: 30%; vertical-align: middle; border: none;">' +
                (showLogo && compLogoBase64 ? 
                  '<div style="background: white; border-radius: ' + logoBorderRadius + '; padding: 5px; display: inline-block; overflow: hidden; text-align: center;">' + logoHTML + '</div>' : 
                  logoHTML
                ) +
              '</td>' +
              '<td style="width: 70%; text-align: right; vertical-align: middle; color: white; border: none;">' +
                '<h2 style="margin: 0; font-size: ' + companyNameSize + '; font-weight: bold; color: white;">' + compName + '</h2>' +
                '<p style="margin: 2px 0 0 0; font-size: 11px; color: white;">' + compAddress + (compGst && gstEnabled ? ' | GSTIN: ' + compGst : '') + '</p>' +
                '<p style="margin: 2px 0 0 0; font-size: 11px; color: white;">Phone no.: ' + compMobile + (compEmail ? ' Email: ' + compEmail : '') + '</p>' +
              '</td>' +
            '</tr>' +
          '</table>' +
          '<div style="margin-top: 10px;">' + checkBoxesHtml + '</div>' +
          '<div style="text-align: center; margin: 15px 0;">' +
            '<h2 style="margin: 0; font-size: ' + docTypeSize + '; font-weight: bold; color: ' + primaryColor + '; text-transform: uppercase;">' + docTypeLabel + '</h2>' +
          '</div>' +
          '<table style="width: 100%; border-collapse: collapse; margin-bottom: 10px; border: none;" border="0">' +
            '<tr>' +
              '<td style="width: 50%; vertical-align: top; border: none;">' +
                '<div style="background: ' + primaryColor + '; color: white; padding: 4px 8px; font-weight: bold; font-size: 12px; display: inline-block; border-radius: 2px;">Bill To</div>' +
                '<div style="padding: 4px 0;">' + billToHtml + '</div>' +
              '</td>' +
              '<td style="width: 50%; text-align: right; vertical-align: top; font-size: 12px; color: ' + primaryColor + '; border: none;">' +
                invoiceDetailsHtml +
              '</td>' +
            '</tr>' +
          '</table>' +
          '<table style="width: 100%; border-collapse: collapse; font-size: 11px; margin-bottom: 10px;">' +
            '<thead>' + tableHeaderHtml + '</thead>' +
            '<tbody>' + itemRowsHtml + totalRowHtml + '</tbody>' +
          '</table>' +
          '<table style="width: 100%; border-collapse: collapse; margin-top: 20px; border: none;" border="0">' +
            '<tr>' +
              '<td style="width: 48%; vertical-align: top; border: none;">' +
                '<div style="background: ' + primaryColor + '; color: white; padding: 4px 8px; font-weight: bold; font-size: 11px;">Invoice Amount In Words</div>' +
                '<div style="padding: 6px 0; font-size: 11px; font-weight: bold;">' + amtInWords + '</div>' +
                savedDiscountHtml +
                '<div style="background: ' + primaryColor + '; color: white; padding: 4px 8px; font-weight: bold; font-size: 11px; margin-top: 10px;">Payment mode</div>' +
                '<div style="padding: 6px 0; font-size: 11px; font-weight: bold;">' + (doc.PaymentMethod || 'N/A') + ' (' + (doc.PaymentStatus || 'Unpaid') + ')</div>' +
              '</td>' +
              '<td style="width: 4%; border: none;"></td>' +
              '<td style="width: 48%; vertical-align: top; border: none;">' +
                '<div style="background: ' + primaryColor + '; color: white; padding: 4px 8px; font-weight: bold; font-size: 11px;">Amounts:</div>' +
                getTotalsTableHtml('Classic M1') +
              '</td>' +
            '</tr>' +
          '</table>' +
          dynamicFooterTableHtml +
          ackHtml +
        '</div>';
    }

    // ===================================
    // THEME 2: CLASSIC M2
    // ===================================
    else if (regTheme === 'Classic M2') {
      contentHtml = 
        '<div style="font-family: \'Segoe UI\', Arial, sans-serif; color: #000; margin: 0; padding: 0; background: #FFF; box-sizing: border-box; position: relative;">' +
          '<table style="width: 100%; border-collapse: collapse; margin-bottom: 20px; border: none;" border="0">' +
            '<tr>' +
              '<td style="text-align: right; font-size: 10px; border: none;">' +
                'Original <span style="width:10px; height:10px; border:1px solid #333; display:inline-block; margin-left:3px; vertical-align:middle;"></span> &nbsp;&nbsp;&nbsp;' +
                'Duplicate <span style="width:10px; height:10px; border:1px solid #333; display:inline-block; margin-left:3px; vertical-align:middle;"></span> &nbsp;&nbsp;&nbsp;' +
                'Triplicate <span style="width:10px; height:10px; border:1px solid #333; display:inline-block; margin-left:3px; vertical-align:middle;"></span>' +
              '</td>' +
            '</tr>' +
          '</table>' +
          '<table style="width: 100%; border-collapse: collapse; margin-bottom: 20px; border: none;" border="0">' +
            '<tr>' +
              '<td style="width: 70%; vertical-align: top; border: none;">' +
                '<h2 style="margin: 0; font-size: ' + companyNameSize + '; font-weight: bold; color: black;">' + compName + '</h2>' +
                '<p style="margin: 4px 0 0 0; font-size: 11px; color: #333;">' + compAddress + (compGst && gstEnabled ? ' | GSTIN: ' + compGst : '') + '</p>' +
                '<p style="margin: 2px 0 0 0; font-size: 11px; color: #333;">Phone no. : ' + compMobile + (compEmail ? ' Email: ' + compEmail : '') + '</p>' +
              '</td>' +
              '<td style="width: 30%; text-align: right; vertical-align: top; border: none;">' +
                (showLogo && compLogoBase64 ? 
                  '<div style="background: white; border-radius: ' + logoBorderRadius + '; padding: 5px; display: inline-block; overflow: hidden; text-align: center;">' + logoHTML + '</div>' : 
                  logoHTML
                ) +
              '</td>' +
            '</tr>' +
          '</table>' +
          '<div style="text-align: center; margin: 25px 0;">' +
            '<h2 style="margin: 0; font-size: ' + docTypeSize + '; font-weight: bold; color: ' + primaryColor + '; text-transform: uppercase;">' + docTypeLabel + '</h2>' +
          '</div>' +
          '<table style="width: 100%; border-collapse: collapse; margin-bottom: 10px; border: none;" border="0">' +
            '<tr>' +
              '<td style="width: 50%; vertical-align: top; border: none;">' +
                '<div style="font-weight: bold; font-size: 12px; margin-bottom: 6px;">Bill To</div>' +
                billToHtml +
              '</td>' +
              '<td style="width: 50%; text-align: right; vertical-align: top; font-size: 12px; font-weight: bold; border: none;">' +
                invoiceDetailsHtml +
              '</td>' +
            '</tr>' +
          '</table>' +
          '<table style="width: 100%; border-collapse: collapse; font-size: 11px; margin-bottom: 20px;">' +
            '<thead>' + tableHeaderHtml + '</thead>' +
            '<tbody>' + itemRowsHtml + totalRowHtml + '</tbody>' +
          '</table>' +
          '<table style="width: 100%; border-collapse: collapse; border: none;" border="0">' +
            '<tr>' +
              '<td style="width: 48%; vertical-align: top; border: none;">' +
                '<div style="font-weight: bold; font-size: 11px; color: #666; margin-bottom: 4px;">Invoice Amount In Words</div>' +
                '<div style="padding: 6px; font-size: 11px; background: #f9f9f9; color: #000; font-weight: bold;">' + amtInWords + '</div>' +
                savedDiscountHtml +
              '</td>' +
              '<td style="width: 4%; border: none;"></td>' +
              '<td style="width: 48%; vertical-align: top; border: none;">' +
                getTotalsTableHtml('Classic M2') +
              '</td>' +
            '</tr>' +
          '</table>' +
          dynamicFooterTableHtml +
          ackHtml +
        '</div>';
    }

    // ===================================
    // THEME 3: CLASSIC M3
    // ===================================
    else if (regTheme === 'Classic M3') {
      contentHtml = 
        '<div style="font-family: \'Segoe UI\', Arial, sans-serif; color: #000; margin: 0; padding: 0; background: #FFF; box-sizing: border-box; position: relative;">' +
          '<table style="width: 100%; border-collapse: collapse; margin-bottom: 10px; border: none;" border="0">' +
            '<tr>' +
              '<td style="width: 50%; vertical-align: middle; border: none;">' +
                '<h2 style="margin: 0; font-size: ' + docTypeSize + '; font-weight: bold; text-transform: uppercase;">' + docTypeLabel + '</h2>' +
              '</td>' +
              '<td style="width: 50%; text-align: right; vertical-align: middle; border: none;">' +
                checkBoxesHtml +
              '</td>' +
            '</tr>' +
          '</table>' +
          '<div style="border: 1px solid #000;">' +
            '<table style="width: 100%; border-collapse: collapse; border-bottom: 1px solid #000; border-left: none; border-right: none; border-top: none;" border="0">' +
              '<tr>' +
                '<td style="width: 50%; padding: 10px; border-right: 1px solid #000; vertical-align: middle;">' +
                  '<table style="width: 100%; border-collapse: collapse; border: none;" border="0">' +
                    '<tr>' +
                      '<td style="width: 30%; border: none; vertical-align: middle;">' + logoHTML + '</td>' +
                      '<td style="width: 70%; border: none; vertical-align: middle; padding-left: 10px;">' +
                        '<h2 style="margin: 0; font-size: 20px; font-weight: bold;">' + compName + '</h2>' +
                        '<p style="margin: 4px 0 0 0; font-size: 10px;">' + compAddress + (compGst && gstEnabled ? ' | GSTIN: ' + compGst : '') + '</p>' +
                        '<p style="margin: 2px 0 0 0; font-size: 10px;">Phone no.: ' + compMobile + (compEmail ? ' Email: ' + compEmail : '') + '</p>' +
                      '</td>' +
                    '</tr>' +
                  '</table>' +
                '</td>' +
                '<td style="width: 50%; padding: 0; vertical-align: top;">' +
                  '<table style="width: 100%; border-collapse: collapse; height: 100%; border: none;" border="0">' +
                    '<tr>' +
                      '<td style="width: 50%; padding: 10px; border-right: 1px solid #000; border-bottom: none; border-top: none; border-left: none; vertical-align: top;">' +
                        '<div style="font-size: 10px; color:#555;">' + docTypeLabel + ' No.</div>' +
                        '<div style="font-weight: bold; font-size: 12px; margin-top: 4px;">' + doc.DocNumber + '</div>' +
                      '</td>' +
                      '<td style="width: 50%; padding: 10px; vertical-align: top; border: none;">' +
                        '<div style="font-size: 10px; color:#555;">Date</div>' +
                        '<div style="font-weight: bold; font-size: 12px; margin-top: 4px;">' + formattedDate + '</div>' +
                        (doc.DueDate ? '<div style="font-size: 9px; color:#E53E3E; margin-top: 5px; font-weight:bold;">Due: ' + doc.DueDate + '</div>' : '') +
                      '</td>' +
                    '</tr>' +
                  '</table>' +
                '</td>' +
              '</tr>' +
            '</table>' +
            '<div style="padding: 10px; border-bottom: 1px solid #000;">' +
              '<div style="font-size: 10px; margin-bottom: 5px; font-weight: bold; text-transform: uppercase; color: #555;">Bill To</div>' +
              billToHtml +
            '</div>' +
            '<table style="width: 100%; border-collapse: collapse; font-size: 10px; border-bottom: 1px solid #000;">' +
              '<thead>' + tableHeaderHtml + '</thead>' +
              '<tbody>' + itemRowsHtml + totalRowHtml + '</tbody>' +
            '</table>' +
            '<table style="width: 100%; border-collapse: collapse; border-bottom: 1px solid #000; border-left:none; border-right:none; border-top:none;" border="0">' +
              '<tr>' +
                '<td style="width: 50%; border-right: 1px solid #000; padding: 0; vertical-align: top;">' +
                  '<div style="padding: 10px; border-bottom: 1px solid #ccc; font-size: 10px; line-height: 1.4;">Invoice Amount In Words<br><span style="font-weight: bold; font-size: 11px;">' + amtInWords + '</span>' + savedDiscountHtml + '</div>' +
                  '<div style="padding: 10px; font-size: 10px; line-height: 1.4;">Payment mode<br><span style="font-weight: bold; font-size: 11px;">' + (doc.PaymentMethod || 'N/A') + ' (' + (doc.PaymentStatus || 'Unpaid') + ')</span></div>' +
                '</td>' +
                '<td style="width: 50%; padding: 0; vertical-align: top;">' +
                  getTotalsTableHtml('Classic M3') +
                '</td>' +
              '</tr>' +
            '</table>' +
            '<table style="width: 100%; border-collapse: collapse; border: none;" border="0">' +
              '<tr>' +
                '<td style="width: 40%; border-right: 1px solid #000; padding: 10px; font-size: 10px; vertical-align: top;">' +
                  tcBlockHtml +
                  '<div style="margin-top: 10px;">' + qrImgHtml + '</div>' +
                '</td>' +
                '<td style="width: 60%; padding: 0; vertical-align: top;">' +
                  '<table style="width: 100%; border-collapse: collapse; border: none;" border="0">' +
                    '<tr>' +
                      '<td style="padding: 10px; border-bottom: 1px solid #ccc; font-size: 10px; vertical-align: top; border-top:none; border-left:none; border-right:none;">' +
                        bankBlockHtml +
                      '</td>' +
                    '</tr>' +
                    '<tr>' +
                      '<td style="padding: 10px; text-align: center; border: none;">' +
                        signatoryHTML +
                      '</td>' +
                    '</tr>' +
                  '</table>' +
                '</td>' +
              '</tr>' +
            '</table>' +
          '</div>' +
          ackHtml +
        '</div>';
    }

    // ===================================
    // THEME 4: CLASSIC M4
    // ===================================
    else if (regTheme === 'Classic M4') {
      contentHtml = 
        '<div style="font-family: \'Segoe UI\', Arial, sans-serif; color: #000; margin: 0; padding: 0; background: #FFF; box-sizing: border-box; position: relative;">' +
          '<table style="width: 100%; border-collapse: collapse; margin-bottom: 10px; border: none;" border="0">' +
            '<tr>' +
              '<td style="width: 50%; vertical-align: middle; border: none;">' +
                '<h2 style="margin: 0; font-size: ' + docTypeSize + '; font-weight: bold; text-transform: uppercase;">' + docTypeLabel + '</h2>' +
              '</td>' +
              '<td style="width: 50%; text-align: right; vertical-align: middle; border: none;">' +
                checkBoxesHtml +
              '</td>' +
            '</tr>' +
          '</table>' +
          '<div style="border: 1px solid #000;">' +
            '<table style="width: 100%; border-collapse: collapse; border-bottom: 1px solid #000; border-top:none; border-left:none; border-right:none;" border="0">' +
              '<tr>' +
                '<td style="width: 50%; padding: 10px; border-right: 1px solid #000; vertical-align: middle;">' +
                  logoHTML +
                '</td>' +
                '<td style="width: 50%; padding: 10px; text-align: right; vertical-align: middle;">' +
                  '<h2 style="margin: 0; font-size: ' + companyNameSize + '; font-weight: bold;">' + compName + '</h2>' +
                  '<p style="margin: 4px 0 0 0; font-size: 10px; color:#333;">' + compAddress + (compGst && gstEnabled ? ' | GSTIN: ' + compGst : '') + '</p>' +
                  '<p style="margin: 2px 0 0 0; font-size: 10px; color:#333;">Phone no.: ' + compMobile + (compEmail ? ' Email: ' + compEmail : '') + '</p>' +
                '</td>' +
              '</tr>' +
            '</table>' +
            '<table style="width: 100%; border-collapse: collapse; border-bottom: 1px solid #000; border-top:none; border-left:none; border-right:none;" border="0">' +
              '<tr>' +
                '<td style="width: 50%; padding: 5px; border-right: 1px solid #000; vertical-align: top;">' +
                  '<div style="background: ' + primaryColor + '; color: white; padding: 2px 5px; font-size: 10px; font-weight: bold; display:inline-block; border-radius:1px;">Bill To</div>' +
                  '<div style="padding: 5px; margin-top:3px;">' + billToHtml + '</div>' +
                '</td>' +
                '<td style="width: 50%; padding: 10px; text-align: right; vertical-align: top;">' +
                  invoiceDetailsHtml +
                '</td>' +
              '</tr>' +
            '</table>' +
            '<table style="width: 100%; border-collapse: collapse; font-size: 10px; border-bottom: 1px solid #000;">' +
              '<thead>' + tableHeaderHtml + '</thead>' +
              '<tbody>' + itemRowsHtml + totalRowHtml + '</tbody>' +
            '</table>' +
            '<table style="width: 100%; border-collapse: collapse; border-bottom: 1px solid #000; border-top:none; border-left:none; border-right:none;" border="0">' +
              '<tr>' +
                '<td style="width: 50%; border-right: 1px solid #000; padding: 0; vertical-align: top;">' +
                  '<div style="background: ' + primaryColor + '; color: white; padding: 3px 5px; font-size: 10px; font-weight: bold; border-bottom: 1px solid #000;">Invoice Amount In Words</div>' +
                  '<div style="padding: 5px; font-size: 10px; border-bottom: 1px solid #ccc; font-weight: bold;">' + amtInWords + '</div>' +
                  savedDiscountHtml +
                  '<div style="background: ' + primaryColor + '; color: white; padding: 3px 5px; font-size: 10px; font-weight: bold; border-bottom: 1px solid #000;">Payment mode</div>' +
                  '<div style="padding: 5px; font-size: 10px; font-weight: bold;">' + (doc.PaymentMethod || 'N/A') + ' (' + (doc.PaymentStatus || 'Unpaid') + ')</div>' +
                '</td>' +
                '<td style="width: 50%; padding: 0; vertical-align: top;">' +
                  getTotalsTableHtml('Classic M4') +
                '</td>' +
              '</tr>' +
            '</table>' +
            '<table style="width: 100%; border-collapse: collapse; border: none;" border="0">' +
              '<tr>' +
                '<td style="width: 35%; border-right: 1px solid #000; padding: 0; vertical-align: top;">' +
                  tcBlockHtml +
                  bankBlockHtml +
                '</td>' +
                '<td style="width: 25%; border-right: 1px solid #000; padding: 10px; text-align: center; vertical-align: top;">' +
                  qrImgHtml +
                '</td>' +
                '<td style="width: 40%; padding: 10px; text-align: center; vertical-align: top;">' +
                  signatoryHTML +
                '</td>' +
              '</tr>' +
            '</table>' +
          '</div>' +
          ackHtml +
        '</div>';
    }

    // ===================================
    // THEME 5: MODERN M1
    // ===================================
    else if (regTheme === 'Modern M1') {
      contentHtml = 
        '<div style="font-family: \'Segoe UI\', Arial, sans-serif; color: #000; margin: 0; padding: 0; background: #FFF; box-sizing: border-box; position: relative;">' +
          '<table style="width: 100%; border-collapse: collapse; margin-bottom: 10px; border: none;" border="0">' +
            '<tr>' +
              '<td style="text-align: right; font-size: 10px; border: none;">' +
                'Original <span style="width:10px; height:10px; border:1px solid #333; display:inline-block; margin-left:3px; vertical-align:middle;"></span> &nbsp;&nbsp;&nbsp;' +
                'Duplicate <span style="width:10px; height:10px; border:1px solid #333; display:inline-block; margin-left:3px; vertical-align:middle;"></span> &nbsp;&nbsp;&nbsp;' +
                'Triplicate <span style="width:10px; height:10px; border:1px solid #333; display:inline-block; margin-left:3px; vertical-align:middle;"></span>' +
              '</td>' +
            '</tr>' +
          '</table>' +
          '<table style="width: 100%; border-collapse: collapse; margin-bottom: 20px; border: none;" border="0">' +
            '<tr>' +
              '<td style="width: 60%; vertical-align: top; border: none;">' +
                '<div style="background: ' + primaryColor + '; color: white; padding: 10px 15px; display: inline-block; text-transform: uppercase; font-size: ' + docTypeSize + '; font-weight: bold; letter-spacing: 1px; margin-bottom: 15px; border-radius:2px;">' + docTypeLabel + '</div>' +
                '<h2 style="margin: 0; font-size: ' + companyNameSize + '; font-weight: normal; color: ' + primaryColor + ';">' + compName + '</h2>' +
              '</td>' +
              '<td style="width: 40%; text-align: right; vertical-align: top; border: none;">' + logoHTML + '</td>' +
            '</tr>' +
          '</table>' +
          '<table style="width: 100%; border-collapse: collapse; font-size: 10px; margin-bottom: 20px; border-bottom: 1px solid #ddd; padding-bottom: 10px; border-top:none; border-left:none; border-right:none;" border="0" cellpadding="5">' +
            '<tr>' +
              '<td style="width: 33%; border: none;">📞 ' + compMobile + '</td>' +
              '<td style="width: 33%; border: none;">✉ ' + compEmail + '</td>' +
              '<td style="width: 34%; border: none;">📍 ' + compAddress + (compGst && gstEnabled ? ' | GST: ' + compGst : '') + '</td>' +
            '</tr>' +
          '</table>' +
          '<table style="width: 100%; border-collapse: collapse; margin-bottom: 20px; border: none;" border="0">' +
            '<tr>' +
              '<td style="width: 48%; vertical-align: top; border: none;">' +
                '<div style="color: ' + primaryColor + '; font-size: 12px; margin-bottom: 5px; font-weight: bold;">' + docTypeLabel + ' Details:</div>' +
                '<table style="width: 100%; border-collapse: collapse; font-size: 11px; border: none;" border="0" cellpadding="2">' +
                  '<tr><td style="width: 40%; border: none; color:#555;">No. :</td><td style="width: 60%; border: none; font-weight: bold;">' + doc.DocNumber + '</td></tr>' +
                  '<tr><td style="width: 40%; border: none; color:#555;">Date:</td><td style="width: 60%; border: none;">' + formattedDate + '</td></tr>' +
                  (doc.DueDate ? '<tr><td style="width: 40%; border: none; color:#E53E3E; font-weight:bold;">Due:</td><td style="width: 60%; border: none; color:#E53E3E; font-weight:bold;">' + doc.DueDate + '</td></tr>' : '') +
                '</table>' +
              '</td>' +
              '<td style="width: 4%; border: none;"></td>' +
              '<td style="width: 48%; vertical-align: top; border: none;">' +
                '<div style="color: ' + primaryColor + '; font-size: 12px; margin-bottom: 5px; font-weight: bold;">Bill To:</div>' +
                billToHtml +
              '</td>' +
            '</tr>' +
          '</table>' +
          '<table style="width: 100%; border-collapse: collapse; font-size: 11px; margin-bottom: 20px;">' +
            '<thead>' + tableHeaderHtml + '</thead>' +
            '<tbody>' + itemRowsHtml + totalRowHtml + '</tbody>' +
          '</table>' +
          '<table style="width: 100%; border-collapse: collapse; border: none;" border="0">' +
            '<tr>' +
              '<td style="width: 48%; vertical-align: top; border: none;">' +
                '<div style="color: ' + primaryColor + '; font-size: 12px; margin-bottom: 5px; font-weight: bold;">Invoice Amount In Words</div>' +
                '<div style="font-size: 11px; margin-bottom: 15px; font-weight: bold;">' + amtInWords + '</div>' +
                savedDiscountHtml +
              '</td>' +
              '<td style="width: 4%; border: none;"></td>' +
              '<td style="width: 48%; vertical-align: top; border: none;">' +
                getTotalsTableHtml('Modern M1') +
              '</td>' +
            '</tr>' +
          '</table>' +
          dynamicFooterTableHtml +
          ackHtml +
        '</div>';
    }

    // ===================================
    // THEME 6: PREMIUM MODEL EXAMPLE
    // ===================================
    else {
      contentHtml = 
        '<div style="font-family: \'Segoe UI\', Arial, sans-serif; color: #000; margin: 0; background: #FFF; box-sizing: border-box; position: relative; padding-bottom: 40px;">' +
          '<table style="width: 100%; border-collapse: collapse; margin-bottom: 10px; border: none;" border="0">' +
            '<tr>' +
              '<td style="text-align: right; font-size: 9px; border: none; padding-right:15px;">' +
                'Original <span style="width:10px; height:10px; border:1px solid #333; display:inline-block; margin-left:3px; vertical-align:middle;"></span> &nbsp;&nbsp;&nbsp;' +
                'Duplicate <span style="width:10px; height:10px; border:1px solid #333; display:inline-block; margin-left:3px; vertical-align:middle;"></span> &nbsp;&nbsp;&nbsp;' +
                'Triplicate <span style="width:10px; height:10px; border:1px solid #333; display:inline-block; margin-left:3px; vertical-align:middle;"></span>' +
              '</td>' +
            '</tr>' +
          '</table>' +
          
          '<!-- Premium Header Band -->' +
          '<table style="width: 100%; border-collapse: collapse; background-color: ' + primaryColor + '; border: none; margin-bottom: 15px;" border="0">' +
            '<tr>' +
              '<td style="padding: 15px; color: white; border: none; width: 60%; background: #2b333e; border-bottom-right-radius: 40px; font-family:\'Segoe UI\', Arial, sans-serif;">' +
                '<table style="width: 100%; border-collapse: collapse; border: none;" border="0">' +
                  '<tr>' +
                    '<td style="width: 65px; border: none; vertical-align: middle;">' +
                      '<div style="background: white; border-radius: ' + logoBorderRadius + '; padding: 5px; display: inline-block; overflow: hidden; width: 55px; height: 55px; text-align:center; vertical-align:middle;">' + logoHTML + '</div>' +
                    '</td>' +
                    '<td style="padding-left: 15px; border: none; color: white; vertical-align: middle;">' +
                      '<div style="font-size: 11px; font-weight: bold;">📞 ' + compMobile + '</div>' +
                      '<div style="font-size: 10px; margin-top: 3px;">✉ ' + compEmail + '</div>' +
                    '</td>' +
                  '</tr>' +
                '</table>' +
              '</td>' +
              '<td style="padding: 15px; color: white; text-align: right; vertical-align: middle; border: none; width: 40%; font-size: 10px; font-family:\'Segoe UI\', Arial, sans-serif;">' +
                '📍 ' + compAddress + (compGst && gstEnabled ? '<br>GSTIN: ' + compGst : '') +
              '</td>' +
            '</tr>' +
          '</table>' +
          
          '<table style="width: 100%; border-collapse: collapse; margin-bottom: 15px; border: none;" border="0" cellpadding="0">' +
            '<tr>' +
              '<td style="width: 50%; color: ' + primaryColor + '; border: none; vertical-align: bottom;">' +
                '<h2 style="margin: 0; font-size: ' + companyNameSize + '; font-weight: bold; text-transform: uppercase;">' + compName + '</h2>' +
              '</td>' +
              '<td style="width: 50%; text-align: right; border: none; vertical-align: bottom;">' +
                '<h1 style="margin: 0; font-size: ' + docTypeSize + '; font-weight: bold; text-transform: uppercase; color: #2d3748;">' + docTypeLabel + '</h1>' +
              '</td>' +
            '</tr>' +
          '</table>' +
          
          '<table style="width: 100%; border-collapse: collapse; margin-bottom: 20px; border: none;" border="0">' +
            '<tr>' +
              '<td style="width: 55%; vertical-align: top; border: none;">' +
                '<div style="color: ' + primaryColor + '; font-size: 12px; margin-bottom: 5px; font-weight: bold;">Bill To:</div>' +
                billToHtml +
              '</td>' +
              '<td style="width: 45%; vertical-align: top; border: none;">' +
                '<table style="width: 100%; border-collapse: collapse; font-size: 11px; border: none;" border="0" cellpadding="3">' +
                  '<tr><td style="font-weight: bold; width: 45%; border: none;">' + docTypeLabel + ' No.:</td><td style="text-align: right; border: none;">' + doc.DocNumber + '</td></tr>' +
                  '<tr><td style="font-weight: bold; border: none;">Date:</td><td style="text-align: right; border: none;">' + formattedDate + '</td></tr>' +
                  (doc.DueDate ? '<tr><td style="font-weight: bold; color:#E53E3E; border: none;">Due Date:</td><td style="text-align: right; color:#E53E3E; font-weight:bold; border: none;">' + doc.DueDate + '</td></tr>' : '') +
                '</table>' +
              '</td>' +
            '</tr>' +
          '</table>' +
          
          '<div style="margin-top: 20px;">' +
            '<table style="width: 100%; border-collapse: collapse; font-size: 10px;">' +
              '<thead>' + tableHeaderHtml + '</thead>' +
              '<tbody>' + itemRowsHtml + totalRowHtml + '</tbody>' +
            '</table>' +
          '</div>' +
          
          '<table style="width: 100%; border-collapse: collapse; border: none;" border="0">' +
            '<tr>' +
              '<td style="width: 55%; vertical-align: top; border: none;">' +
                '<div style="color: ' + primaryColor + '; font-size: 11px; margin-bottom: 3px; font-weight: bold;">Invoice Amount In Words</div>' +
                '<div style="font-size: 10px; margin-bottom: 10px; font-weight: bold;">' + amtInWords + '</div>' +
                savedDiscountHtml +
              '</td>' +
              '<td style="width: 5%; border: none;"></td>' +
              '<td style="width: 40%; vertical-align: top; border: none;">' +
                getTotalsTableHtml('Premium Model Example') +
              '</td>' +
            '</tr>' +
          '</table>' +
          
          dynamicFooterTableHtml +
          ackHtml +
          
          '<!-- Premium Footer Band -->' +
          '<table style="width: 100%; border-collapse: collapse; margin-top: 30px; background-color: ' + primaryColor + '; border: none;" border="0">' +
            '<tr>' +
              '<td style="border: none; height: 25px;"></td>' +
              '<td style="width: 40%; background: #2b333e; border-top-left-radius: 30px; border: none; height: 25px;"></td>' +
            '</tr>' +
          '</table>' +
        '</div>';
    }

    var html = '<!DOCTYPE html><html><head><meta charset="utf-8"><style>' +
      '@page{size:A4;margin:' + marginTop + 'mm ' + marginRight + 'mm ' + marginBottom + 'mm ' + marginLeft + 'mm;}' +
      'body{font-family:Arial,Helvetica,sans-serif;color:#2D3748;margin:0;padding:0;background:#fff;-webkit-print-color-adjust:exact !important;print-color-adjust:exact !important;}' +
      'table{border-collapse:collapse;width:100%;-webkit-print-color-adjust:exact !important;print-color-adjust:exact !important;}' +
      '*{-webkit-print-color-adjust:exact !important;print-color-adjust:exact !important;}' +
      '</style></head><body>' + contentHtml + '</body></html>';

    var blob = Utilities.newBlob(html, 'text/html', 'UTF-8').getAs(MimeType.PDF);
    blob.setName(docTypeLabel.replace(/\s+/g,'_') + '_' + docNumber + '.pdf');
    return { success: true, base64: Utilities.base64Encode(blob.getBytes()), filename: blob.getName() };
  } catch(e) {
    console.error('downloadInvoicePDF Error:', e);
    return { success: false, message: e.message };
  }
}

function convertNumberToWords(num) {
  num = parseFloat(num);
  if (isNaN(num) || num <= 0) return 'Rupees Zero only';
  var a = ['', 'One ', 'Two ', 'Three ', 'Four ', 'Five ', 'Six ', 'Seven ', 'Eight ', 'Nine ', 'Ten ', 'Eleven ', 'Twelve ', 'Thirteen ', 'Fourteen ', 'Fifteen ', 'Sixteen ', 'Seventeen ', 'Eighteen ', 'Nineteen '];
  var b = ['', '', 'Twenty', 'Thirty', 'Forty', 'Fifty', 'Sixty', 'Seventy', 'Eighty', 'Ninety'];

  function numToWords(n) {
    if (n < 20) return a[n];
    if (n < 100) return b[Math.floor(n / 10)] + (n % 10 !== 0 ? ' ' + a[n % 10] : '');
    if (n < 1000) return a[Math.floor(n / 100)] + 'Hundred ' + (n % 100 !== 0 ? 'and ' + numToWords(n % 100) : '');
    return '';
  }

  var n = Math.floor(num);
  if (n === 0) return 'Rupees Zero only';

  var str = '';
  var crore = Math.floor(n / 10000000);
  n %= 10000000;
  var lakh = Math.floor(n / 100000);
  n %= 100000;
  var thousand = Math.floor(n / 1000);
  n %= 1000;

  if (crore > 0) str += numToWords(crore) + 'Crore ';
  if (lakh > 0) str += numToWords(lakh) + 'Lakh ';
  if (thousand > 0) str += numToWords(thousand) + 'Thousand ';
  if (n > 0) str += numToWords(n);

  var outStr = 'Rupees ' + str;
  
  var paisa = Math.round((num - Math.floor(num)) * 100);
  if (paisa > 0) {
    outStr += 'and ' + numToWords(paisa) + 'Paise ';
  }
  outStr += 'only';
  return outStr.replace(/\s+/g, ' ').trim();
}

function getAcknowledgementPageHtml_(params) {
  var regTheme = params.regTheme;
  var primaryColor = params.primaryColor;
  var compName = params.compName;
  var compAddress = params.compAddress;
  var compGst = params.compGst;
  var compMobile = params.compMobile;
  var compEmail = params.compEmail;
  var compLogoBase64 = params.compLogoBase64;
  var logoBorderRadius = params.logoBorderRadius;
  var logoHTML = params.logoHTML;
  var companyNameSize = params.companyNameSize;
  var docTypeSize = params.docTypeSize;
  var docTypeLabel = params.docTypeLabel;
  var doc = params.doc;
  var partyName = params.partyName;
  var partyAddress = params.partyAddress;
  var partyMobile = params.partyMobile;
  var formattedDocDate = params.formattedDocDate;
  var grandTotalVal = params.grandTotalVal;
  var settings = params.settings;
  var checkBoxesHtml = params.checkBoxesHtml;
  var showSignatory = (settings.RegularShowSignatory !== 'false');
  var signImage = settings.SignatureImage || settings.CompanySignature || '';
  var signImageBase64 = convertUrlToBase64_(signImage);

  var useDecimals = (settings.AmountWithDecimals !== 'false');
  var formattedGrandTotal = useDecimals ? grandTotalVal.toFixed(2) : String(Math.round(grandTotalVal));

  var headerHtml = '';
  var footerWrap = '';
  var footerBand = '';

  if (regTheme === 'Classic M1') {
    headerHtml = 
      '<table style="width: 100%; border-collapse: collapse; background-color: ' + primaryColor + '; color: white; border: none; margin-bottom: 10px;" cellpadding="15" border="0">' +
        '<tr>' +
          '<td style="width: 30%; vertical-align: middle; border: none;">' +
            (compLogoBase64 ? 
              '<div style="background: white; border-radius: ' + logoBorderRadius + '; padding: 5px; display: inline-block; overflow: hidden; text-align: center;">' + logoHTML + '</div>' : 
              logoHTML
            ) +
          '</td>' +
          '<td style="width: 70%; text-align: right; vertical-align: middle; color: white; border: none;">' +
            '<h2 style="margin: 0; font-size: ' + companyNameSize + '; font-weight: bold; color: white;">' + compName + '</h2>' +
            '<p style="margin: 2px 0 0 0; font-size: 11px; color: white;">' + compAddress + (compGst ? ' | GSTIN: ' + compGst : '') + '</p>' +
            '<p style="margin: 2px 0 0 0; font-size: 11px; color: white;">Phone no.: ' + compMobile + (compEmail ? ' Email: ' + compEmail : '') + '</p>' +
          '</td>' +
        '</tr>' +
      '</table>' +
      '<div style="margin-top: 10px;">' + checkBoxesHtml + '</div>';
  }
  else if (regTheme === 'Classic M2') {
    headerHtml = 
      checkBoxesHtml + 
      '<table style="width: 100%; border-collapse: collapse; margin-bottom: 20px; border: none;" border="0">' +
        '<tr>' +
          '<td style="width: 70%; vertical-align: top; border: none;">' +
            '<h2 style="margin: 0; font-size: ' + companyNameSize + '; font-weight: bold; color: black;">' + compName + '</h2>' +
            '<p style="margin: 4px 0 0 0; font-size: 11px; color: #333;">' + compAddress + (compGst ? ' | GSTIN: ' + compGst : '') + '</p>' +
            '<p style="margin: 2px 0 0 0; font-size: 11px; color: #333;">Phone no. : ' + compMobile + (compEmail ? ' Email: ' + compEmail : '') + '</p>' +
          '</td>' +
          '<td style="width: 30%; text-align: right; vertical-align: top; border: none;">' +
            (compLogoBase64 ? 
              '<div style="background: white; border-radius: ' + logoBorderRadius + '; padding: 5px; display: inline-block; overflow: hidden; text-align: center;">' + logoHTML + '</div>' : 
              logoHTML
            ) +
          '</td>' +
        '</tr>' +
      '</table>';
  }
  else if (regTheme === 'Classic M3') {
    headerHtml = 
      '<table style="width: 100%; border-collapse: collapse; margin-bottom: 10px; border: none;" border="0">' +
        '<tr>' +
          '<td style="width: 50%; vertical-align: middle; border: none;">' +
            '<h2 style="margin: 0; font-size: ' + docTypeSize + '; font-weight: bold; text-transform: uppercase;">ACKNOWLEDGEMENT</h2>' +
          '</td>' +
          '<td style="width: 50%; text-align: right; vertical-align: middle; border: none;">' +
            checkBoxesHtml +
          '</td>' +
        '</tr>' +
      '</table>' +
      '<div style="border: 1px solid #000; padding: 10px; box-sizing: border-box;">' +
        '<table style="width: 100%; border-collapse: collapse; border-bottom: 1px solid #000; border-left: none; border-right: none; border-top: none;" border="0">' +
          '<tr>' +
            '<td style="width: 50%; padding: 10px; border-right: 1px solid #000; vertical-align: middle;">' +
              '<table style="width: 100%; border-collapse: collapse; border: none;" border="0">' +
                '<tr>' +
                  '<td style="width: 30%; border: none; vertical-align: middle;">' + logoHTML + '</td>' +
                  '<td style="width: 70%; border: none; vertical-align: middle; padding-left: 10px;">' +
                    '<h2 style="margin: 0; font-size: 20px; font-weight: bold;">' + compName + '</h2>' +
                    '<p style="margin: 4px 0 0 0; font-size: 10px;">' + compAddress + (compGst ? ' | GSTIN: ' + compGst : '') + '</p>' +
                    '<p style="margin: 2px 0 0 0; font-size: 10px;">Phone no.: ' + compMobile + (compEmail ? ' Email: ' + compEmail : '') + '</p>' +
                  '</td>' +
                '</tr>' +
              '</table>' +
            '</td>' +
            '<td style="width: 50%; padding: 10px; vertical-align: top; border: none;">' +
              '<div style="font-size: 10px; color:#555;">Acknowledgement No.</div>' +
              '<div style="font-weight: bold; font-size: 12px; margin-top: 4px;">' + doc.DocNumber + '</div>' +
              '<div style="font-size: 10px; color:#555; margin-top: 5px;">Date</div>' +
              '<div style="font-weight: bold; font-size: 12px; margin-top: 4px;">' + formattedDocDate + '</div>' +
            '</td>' +
          '</tr>' +
        '</table>';
    footerWrap = '</div>';
  }
  else if (regTheme === 'Classic M4') {
    headerHtml = 
      '<table style="width: 100%; border-collapse: collapse; margin-bottom: 10px; border: none;" border="0">' +
        '<tr>' +
          '<td style="width: 50%; vertical-align: middle; border: none;">' +
            '<h2 style="margin: 0; font-size: ' + docTypeSize + '; font-weight: bold; text-transform: uppercase;">ACKNOWLEDGEMENT</h2>' +
          '</td>' +
          '<td style="width: 50%; text-align: right; vertical-align: middle; border: none;">' +
            checkBoxesHtml +
          '</td>' +
        '</tr>' +
      '</table>' +
      '<div style="border: 1px solid #000; padding: 10px; box-sizing: border-box;">' +
        '<table style="width: 100%; border-collapse: collapse; border-bottom: 1px solid #000; border-top:none; border-left:none; border-right:none;" border="0">' +
          '<tr>' +
            '<td style="width: 50%; padding: 10px; border-right: 1px solid #000; vertical-align: middle;">' + logoHTML + '</td>' +
            '<td style="width: 50%; padding: 10px; text-align: right; vertical-align: middle;">' +
              '<h2 style="margin: 0; font-size: ' + companyNameSize + '; font-weight: bold;">' + compName + '</h2>' +
              '<p style="margin: 4px 0 0 0; font-size: 10px; color:#333;">' + compAddress + (compGst ? ' | GSTIN: ' + compGst : '') + '</p>' +
              '<p style="margin: 2px 0 0 0; font-size: 10px; color:#333;">Phone no.: ' + compMobile + (compEmail ? ' Email: ' + compEmail : '') + '</p>' +
            '</td>' +
          '</tr>' +
        '</table>';
    footerWrap = '</div>';
  }
  else if (regTheme === 'Modern M1') {
    headerHtml = 
      '<table style="width: 100%; border-collapse: collapse; margin-bottom: 10px; border: none;" border="0">' +
        '<tr>' +
          '<td style="text-align: right; font-size: 10px; border: none;">' + checkBoxesHtml + '</td>' +
        '</tr>' +
      '</table>' +
      '<table style="width: 100%; border-collapse: collapse; margin-bottom: 20px; border: none;" border="0">' +
        '<tr>' +
          '<td style="width: 60%; vertical-align: top; border: none;">' +
            '<div style="background: ' + primaryColor + '; color: white; padding: 10px 15px; display: inline-block; text-transform: uppercase; font-size: ' + docTypeSize + '; font-weight: bold; letter-spacing: 1px; margin-bottom: 15px; border-radius:2px;">ACKNOWLEDGEMENT</div>' +
            '<h2 style="margin: 0; font-size: ' + companyNameSize + '; font-weight: normal; color: ' + primaryColor + ';">' + compName + '</h2>' +
          '</td>' +
          '<td style="width: 40%; text-align: right; vertical-align: top; border: none;">' + logoHTML + '</td>' +
        '</tr>' +
      '</table>' +
      '<table style="width: 100%; border-collapse: collapse; font-size: 10px; margin-bottom: 20px; border-bottom: 1px solid #ddd; padding-bottom: 10px; border-top:none; border-left:none; border-right:none;" border="0" cellpadding="5">' +
        '<tr>' +
          '<td style="width: 33%; border: none;">📞 ' + compMobile + '</td>' +
          '<td style="width: 33%; border: none;">✉ ' + compEmail + '</td>' +
          '<td style="width: 34%; border: none;">📍 ' + compAddress + (compGst ? ' | GST: ' + compGst : '') + '</td>' +
        '</tr>' +
      '</table>';
  }
  else { // Premium Model Example
    headerHtml = 
      '<table style="width: 100%; border-collapse: collapse; margin-bottom: 10px; border: none;" border="0">' +
        '<tr>' +
          '<td style="text-align: right; font-size: 9px; border: none; padding-right:15px;">' + checkBoxesHtml + '</td>' +
        '</tr>' +
      '</table>' +
      '<table style="width: 100%; border-collapse: collapse; background-color: ' + primaryColor + '; border: none; margin-bottom: 15px;" border="0">' +
        '<tr>' +
          '<td style="padding: 15px; color: white; border: none; width: 60%; background: #2b333e; border-bottom-right-radius: 40px; font-family:\'Segoe UI\', Arial, sans-serif;">' +
            '<table style="width: 100%; border-collapse: collapse; border: none;" border="0">' +
              '<tr>' +
                '<td style="width: 65px; border: none; vertical-align: middle;">' +
                  '<div style="background: white; border-radius: ' + logoBorderRadius + '; padding: 5px; display: inline-block; overflow: hidden; width: 55px; height: 55px; text-align:center; vertical-align:middle;">' + logoHTML + '</div>' +
                '</td>' +
                '<td style="padding-left: 15px; border: none; color: white; vertical-align: middle;">' +
                  '<div style="font-size: 11px; font-weight: bold;">📞 ' + compMobile + '</div>' +
                  '<div style="font-size: 10px; margin-top: 3px;">✉ ' + compEmail + '</div>' +
                '</td>' +
              '</tr>' +
            '</table>' +
          '</td>' +
          '<td style="padding: 15px; color: white; text-align: right; vertical-align: middle; border: none; width: 40%; font-size: 10px; font-family:\'Segoe UI\', Arial, sans-serif;">' +
            '📍 ' + compAddress + (compGst ? '<br>GSTIN: ' + compGst : '') +
          '</td>' +
        '</tr>' +
      '</table>' +
      '<table style="width: 100%; border-collapse: collapse; margin-bottom: 15px; border: none;" border="0" cellpadding="0">' +
        '<tr>' +
          '<td style="width: 50%; color: ' + primaryColor + '; border: none; vertical-align: bottom;">' +
            '<h2 style="margin: 0; font-size: ' + companyNameSize + '; font-weight: bold; text-transform: uppercase;">' + compName + '</h2>' +
          '</td>' +
          '<td style="width: 50%; text-align: right; border: none; vertical-align: bottom;">' +
            '<h1 style="margin: 0; font-size: ' + docTypeSize + '; font-weight: bold; text-transform: uppercase; color: #2d3748;">ACKNOWLEDGEMENT</h1>' +
          '</td>' +
        '</tr>' +
      '</table>';

    footerBand = 
      '<!-- Premium Footer Band -->' +
      '<table style="width: 100%; border-collapse: collapse; margin-top: 30px; background-color: ' + primaryColor + '; border: none;" border="0">' +
        '<tr>' +
          '<td style="border: none; height: 25px;"></td>' +
          '<td style="width: 40%; background: #2b333e; border-top-left-radius: 30px; border: none; height: 25px;"></td>' +
        '</tr>' +
      '</table>';
  }

  var billToBlock = 
    '<table style="width: 100%; border-collapse: collapse; margin-bottom: 25px; border: none; margin-top: 15px;" border="0">' +
      '<tr>' +
        '<td style="width: 55%; vertical-align: top; border: none; font-family:\'Segoe UI\', Arial, sans-serif;">' +
          '<div style="color: ' + primaryColor + '; font-size: 12px; margin-bottom: 5px; font-weight: bold;">Bill To / Receiver Details:</div>' +
          '<div style="font-weight: bold; font-size: 14px; color: #1a202c; margin-bottom: 4px;">' + partyName + '</div>' +
          '<div style="font-size: 11px; line-height: 1.5; color: #2d3748; font-weight: 500;">' +
            (partyMobile ? '<b>Ph:</b> ' + partyMobile + '<br>' : '') +
            '<b>Address:</b> ' + (partyAddress || '-') +
          '</div>' +
        '</td>' +
        '<td style="width: 45%; vertical-align: top; border: none; font-family:\'Segoe UI\', Arial, sans-serif;">' +
          '<div style="color: ' + primaryColor + '; font-size: 12px; margin-bottom: 5px; font-weight: bold;">Document Reference:</div>' +
          '<table style="width: 100%; border-collapse: collapse; font-size: 11px; border: none;" border="0" cellpadding="3">' +
            '<tr><td style="font-weight: bold; width: 45%; border: none;">' + docTypeLabel + ' No.:</td><td style="text-align: right; border: none; font-weight: bold;">' + doc.DocNumber + '</td></tr>' +
            '<tr><td style="font-weight: bold; border: none;">Document Date:</td><td style="text-align: right; border: none;">' + formattedDocDate + '</td></tr>' +
            '<tr><td style="font-weight: bold; border: none;">Amount:</td><td style="text-align: right; border: none; font-weight: bold; color: ' + primaryColor + ';">₹' + formattedGrandTotal + '</td></tr>' +
          '</table>' +
        '</td>' +
      '</tr>' +
    '</table>';

  var declarationBlock = 
    '<div style="margin: 40px 0; padding: 20px; border: 1px dashed ' + primaryColor + '; background-color: #fafafa; border-radius: 4px; font-family:\'Segoe UI\', Arial, sans-serif;">' +
      '<h4 style="margin: 0 0 10px 0; color: ' + primaryColor + '; font-size: 13px; font-weight: bold; text-transform: uppercase;">DECLARATION & RECEIPT</h4>' +
      '<p style="margin: 0; font-size: 12px; line-height: 1.6; color: #2d3748;">' +
        'Received with thanks from <b>' + partyName + '</b>, the sum of <b>Rupees ' + convertNumberToWords(grandTotalVal) + '</b> ' +
        'towards the settlement of <b>' + docTypeLabel + ' No. ' + doc.DocNumber + '</b> dated <b>' + formattedDocDate + '</b>.' +
      '</p>' +
    '</div>';

  var signatoryBlock = 
    '<table style="width: 100%; border-collapse: collapse; margin-top: 60px; margin-bottom: 40px; border: none;" border="0">' +
      '<tr>' +
        '<td style="width: 50%; text-align: left; vertical-align: bottom; border: none;">' +
          '<div style="text-align: left; display: inline-block; min-width: 180px; font-family:\'Segoe UI\', Arial, sans-serif;">' +
            '<div style="height: 60px;"></div>' +
            '<div style="border-top: 1px dotted #333; padding-top: 5px; font-weight: bold; font-size: 11px;">Receiver\'s Seal & Sign</div>' +
          '</div>' +
        '</td>' +
        '<td style="width: 50%; text-align: right; vertical-align: bottom; border: none;">' +
          '<div style="text-align: center; display: inline-block; min-width: 180px; font-family:\'Segoe UI\', Arial, sans-serif;">' +
            (showSignatory ? 
              ('<div style="font-size: 10px; margin-bottom: 5px; color:#555;">For ' + compName + '</div>' +
               (signImageBase64 ? '<img src="' + signImageBase64 + '" style="max-width: 130px; max-height: 50px; object-fit: contain; margin-bottom: 4px;">' : '<div style="height:50px;"></div>') +
               '<div style="border-top: 1px solid #333; padding-top: 5px; font-weight: bold; font-size: 11px;">Authorized Signatory</div>') :
              ('<div style="height: 60px;"></div>' +
               '<div style="border-top: 1px dotted #333; padding-top: 5px; font-weight: bold; font-size: 11px;">Authorized Signatory</div>')
            ) +
          '</div>' +
        '</td>' +
      '</tr>' +
    '</table>';

  return '<div style="page-break-before: always; margin: 0; padding: 0; box-sizing: border-box;">' +
    headerHtml +
    billToBlock +
    declarationBlock +
    signatoryBlock +
    footerWrap +
    footerBand +
  '</div>';
}
