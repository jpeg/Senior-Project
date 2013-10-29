###################################################################
# Jason Gassel
# 10/28/2013
# SendEmail.py
#
# Sends an email including an optional attachment through a Gmail
# account using SMTP.
###################################################################

import os
import smtplib
import mimetypes

from email import encoders
from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart
from email.mime.base import MIMEBase

def sendEmail(recipient, subject, message, username, password, attachmentPath=None):
    email = MIMEMultipart('alternative')
    email['Subject'] = subject
    email['From'] = username
    email['To'] = recipient
    email.attach(MIMEText(message, 'plain'))
    
    if attachmentPath:
        try:
            mimetype, encoding = mimetypes.guess_type(attachmentPath)
            # If unknown type, or compressed file use generic MIME type
            if mimetype is None or encoding is not None:
                mimetype = 'application/octet-stream'
            main, sub = mimetype.split('/', 1)
            attachment = MIMEBase(main, sub)
            
            f = open(attachmentPath, 'rb')
            attachment.set_payload(f.read())
            f.close()
            
            # Compress attached file
            encoders.encode_base64(attachment)
            # Add header, including filename
            attachment.add_header('Content-Disposition', 'attachment', filename = os.path.basename(attachmentPath))
        except Exception as e:
            return False
        email.attach(attachment)
    
    try:
        smtp = smtplib.SMTP()
        smtp.connect('smtp.gmail.com', 587)
        smtp.ehlo()
        smtp.starttls()
        smtp.ehlo()
        smtp.login(username, password)
        smtp.sendmail(username, email['To'], email.as_string())
        smtp.quit()
    except:
        return False
    
    return True

