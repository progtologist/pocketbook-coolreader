
#include <inkview.h>
#include <inkplatform.h>
#include <crengine.h>
#include <crgui.h>
#include <cri18n.h>
#include <lvstream.h>
#include "web.h"
#include "cr3pocketbook.h"
#include "ota_update.h"


/**
 * Check if there is a new version
 *
 * @return  true if new version, false otherwise
 */
bool OTA_isNewVersion() {
    if( !pbNetwork("connect") )
        return false;
    const char * response = web::get(OTA_VERSION).c_str();
    return
        strlen(response) > 5 &&
        strlen(response) <= OTA_VERSION_MAX_LENGTH &&
        strcmp(CR_PB_VERSION, response) != 0;
}

/**
 * Checks if the test link is fine
 *
 * @param  url  url
 *
 * @return  true if ok, false if not
 */
bool OTA_downloadExists(const lString16 url) {
    if( !pbNetwork("connect") )
        return false;
    const char * response = web::get(UnicodeToUtf8(url).c_str()).c_str();
    return
        strlen(response) == strlen(OTA_EXISTS_STR) &&
        strcmp(OTA_EXISTS_STR, response) == 0;
}

/**
 * Gets a device model if the current device is linked
 *
 * @param  deviceModel  current device model number
 *
 * @return  linked device model or empty if not linked
 */
lString16 OTA_getLinkedDevice(const lString16 deviceModel) {
    if( !pbNetwork("connect") )
        return NULL;
    lString16 url = lString16(OTA_LINK_MASK);
    url.replace(lString16("[DEVICE]"), deviceModel);
    const char * response = web::get(UnicodeToUtf8(url).c_str()).c_str();
    if( strlen(response) > 0 && strlen(response) <= OTA_VERSION_MAX_LENGTH )
        return lString16(response);
    return lString16("");
}

/**
 * Generate url from mask and device model number
 *
 * @param  mask         url mask
 * @param  deviceModel  device model number
 *
 * @return  url
 */
lString16 OTA_genUrl(const char *mask, const lString16 deviceModel) {
    lString16 url = lString16(mask);
    url.replace(lString16("[DEVICE]"), deviceModel);
    return url;
}

/**
 * This does the actual updating of the app
 *
 * @param  url  url
 *
 * @return  true if updated, false if error
 */
bool OTA_updateFrom(const lString16 url) {

    CloseProgressbar();
    Message(ICON_INFORMATION,  const_cast<char*>("CoolReader"),
        UnicodeToUtf8(url).c_str(), 5000);

    return false;
}

/**
 * Update progress bar
 *
 * @param  text         text shown
 * @param  progress     0..100
 * @param  deviceModel  device model
 */
void OTA_progress(const char *text, int progress, lString16 deviceModel) {
    lString16 finalText = lString16(text);
    finalText.replace(lString16("[DEVICE]"), deviceModel);
    UpdateProgressbar(UnicodeToUtf8(finalText).c_str(), progress);
}
void OTA_progress(const char *text, int progress) {
    OTA_progress(text, progress, lString16(""));
}

/**
 * Main func to be called for updating
 *
 * @return  true if updated, false if not
 */
bool OTA_update() {

    iv_dialoghandler progressbar;

    OpenProgressbar(ICON_INFORMATION, _("OTA Update"), _("Checking network connection..."), 0, progressbar);

    // Network Connect
    if( !pbNetwork("connect") ) {
        Message(ICON_ERROR,  const_cast<char*>("CoolReader"),
            _("Couldn't connect to the network!"), 2000);
        return false;
    }

    OTA_progress(_("Checking for updates..."), 10);

    if( !OTA_isNewVersion() ) {
        CloseProgressbar();
        Message(ICON_INFORMATION,  const_cast<char*>("CoolReader"),
            _("You have the latest version."), 2000);
        return false;
    }

    // Get device model number
    const lString16 deviceModel = getPbModelNumber();

    OTA_progress(_("Searching update package for [DEVICE]..."), 20, deviceModel);

    // If download exists
    if( OTA_downloadExists( OTA_genUrl(OTA_URL_MASK_TEST, deviceModel) ) ) {

        OTA_progress(_("Downloading package for [DEVICE]..."), 50, deviceModel);

        // Update
        return OTA_updateFrom( OTA_genUrl(OTA_URL_MASK, deviceModel) );
    }

    OTA_progress(_("Searching twin device for [DEVICE]..."), 30, deviceModel);

    // Check if the device is linked to another one
    const lString16 linkedDevice = OTA_getLinkedDevice(deviceModel);
    if( linkedDevice.empty() ) {
        CloseProgressbar();
        Message(ICON_WARNING,  const_cast<char*>("CoolReader"),
            (
            lString8(_("Update is not available for your device!\nDevice model: ")) +
            UnicodeToUtf8(deviceModel)
            ).c_str()
            , 5000);
        return false;
    }

    OTA_progress(_("Searching update package for [DEVICE]..."), 40, linkedDevice);

    // If the device is linked and the download exists
    if( OTA_downloadExists( OTA_genUrl(OTA_URL_MASK_TEST, linkedDevice) ) ) {

        OTA_progress(_("Downloading package for [DEVICE]..."), 50, linkedDevice);

        // Update
        return OTA_updateFrom( OTA_genUrl(OTA_URL_MASK, linkedDevice) );
    }

    // Shouldn't reach this part
    CloseProgressbar();
    Message(ICON_ERROR,  const_cast<char*>("CoolReader"),
        _("Failed updating!"), 2000);
    return false;
}

/**
 * Installs the files from the update archive
 *
 * @return  true if success, false if not
 */
bool OTA_installUpdatePackage() {

    return true;
}

/**
 * Check there is a valid package file in the given directory
 *
 * @return  true if valid, false if not
 */
bool OTA_gotValidPackage() {

    lString16 dir = lString16(OTA_DOWNLOAD_DIR);

    // Open dir
    LVContainerRef dirHandle = LVOpenDirectory(dir);
    if (dirHandle.isNull()) {
        Message(ICON_ERROR,  const_cast<char*>("CoolReader"),
            _("Couldn't open download dir!"), 2000);
        return false;
    }

    lString16 file = lString16(OTA_PACKAGE_NAME);

    // Open file
    LVStreamRef fileHandle = dirHandle->OpenStream(file.c_str(), LVOM_READ);
    if (!fileHandle) {
        Message(ICON_ERROR,  const_cast<char*>("CoolReader"),
            _("Couldn't open downloaded file!"), 2000);
        return false;
    }

    // Open archive
    LVContainerRef archive = LVOpenArchieve( fileHandle );
    if ( archive.isNull() ) {
        Message(ICON_ERROR,  const_cast<char*>("CoolReader"),
            _("Downloaded file is not an archive!"), 2000);
        return false;
    }

    // Open binary
    LVStreamRef binary = archive->OpenStream(L"system/bin/cr3-pb.app", LVOM_READ);
    if ( binary.isNull() ) {
        Message(ICON_ERROR,  const_cast<char*>("CoolReader"),
            _("Invalid update package!"), 2000);
        return false;
    }

    // Opened binary. All fine.
    return true;
}