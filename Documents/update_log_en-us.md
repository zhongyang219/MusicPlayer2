**[简体中文](./update_log.md) | English**

# MusicPlayer2 Update log
## V2.77.1 (2024/08/23)

This update fixed several issues since version 2.77. No new features added.
* Fixed the problem that the "Add to Playlist" submenu in the context menu sometimes abnormally disabled.
* Fixed the problem that the test text was not deleted when clicking the "AB Repeat" button.
* Fixed the problem that the mouse tooltip of the "AB Repeat" button was not correct.
* Fixed the problem that the control in the dialog box is too crowded vertically at 100% zoom ratio.
* Fixed the problem that lyrics text is too big and incomplete when entering mini mode from full screen mode.
* Fixed the problem that the internal album cover of acc files can not be displayed in the property dialog box.

## V2.77 (2024/08/12)

**bug fixes:**

* Fixed the problem that "ALBUMARTIS" and "DISCNUMBER" fields are duplicated when editing the label information of audio files.
* Fixed the problem that the taskbar of Windows 11 not responding.
* Fixed the problem that the tabs of the Options Settings dialog and the self-drawn playlist scroll too fast when using touchpad gestures.
* Fixed the problem that the volume adjustment too fast when using touchpad gestures.
* Fixed the problem that buttons not shown in the interface can still be clicked after the stackElement is switched.
* Fixed the problem that some icons in the interface are not clear when the system scaling ratio is 125% and 150%.
* Fixed the problem that the button in the top right corner overlaps with other elements in the interface when "Use system standard title bar" is enabled.
* Fixed the problem that strings containing numeric prefixes are not sorted by value when the playlist is sorted.
* Fixed the problem that the functions of "Select All", "Select None" and "Select Reverse" in the playlist menu don't work for the self-drawn playlist.
* Fix the problem that tracks can still be deleted by pressing the Delete key after deleting them from the playlist.

**New Features:**

* Added the splitter control in main window and media library window.
* Added the option to ignore audio files that are too short when the media library is updated.
* Added "Auto" for lyrics alignment in lyrics settings. Lyrics alignment is valid for double line display mode.
* Added the option of "Insert at the start of the playlist when adding songs, not at the end".
* Add the function of "Sort by listen time" in the playlist sorting menu.
* When playing from the "Artist", "Album", "All Tracks", etc. in the media library, it will play directly from the media library mode instead of adding it to the [Temporary playlist].
* Added the recently played media library item to playlist dropdown menu
* Added the "Navigation Bar", "Recently Played", "Folder", "Playlist", "My Favorite", "Media Library Items" and "All Tracks" element to the user-defined interface.
* UI "Groove Music Style", "Groove Music Style (Windows 11)" updated, added the left navigation bar.
* Added the UI12.
* Added the "font_size" attribute to "playlist", "playlistIndicator" and "trackInfo" element in user-defined interface.
* Added the "show_text" and "font_size" attributes to the button element in the user-defined interface. The button can display text on the right side of the icon.
* The maximum font size in user-defined interface is increased to 16 pounds.
* The path of a local file or folder is allow to entered in the edit box  in the "Open url" dialog.
* Added the "Mini mode 3" UI.
* Added buttons of "Play", "Add to Playlist", "Add to My Favorite" in the corresponding rows when the mouse is pointed to the playlist.
* If the song in the playlist is in "My Favorite", a red heart icon will be shown on the right side.
* Added the function of "Remove from playlist" in the context menu of the list on the right side of the "Playlist" tab in the "Media Library" dialog box.
* Added the function of "Sort mode" to the interface of "Folder" and "Playlist" in Media Library.
* Removing a track from a playlist displays a confirmation dialog to avoid misoperation.
* Added the "Add to playlist" command to the folder context menu.
* Added the "Add to my favorite" option to "Global shortcut key".
* Added support for multi-selection of self-drawn playlists. Added support for selecting all by pressing Ctrl+A.
* After successfully executing the "Add to Playlist" command, a success message will be displayed in the center of the interface.
* Added the option of whether to allow "Global Mouse Wheel Volume Adjustment" in General Settings.

**Update Note:**

* After version 2.77, the translation files are stored in the "language" directory. When updating, please make sure to copy the language directory to the MusicPlayer2 directory as well.

* The layout of the controls in the Option Settings dialog has changed since version 2.77, this is to ensure that the text is fully displayed in different languages.

* About the white border at the top of the title bar on Windows 10/11, an option has been added in this version to remove this white border:

  Just open the configuration file "config.ini" while making sure the program exits, find the "remove_titlebar_top_frame" option under [config] and change it to "true". This feature is turned off by default and not added to the UI as there are still some issues with the window flickering when it gains and loses focus.

## V2.76.1 (2023/04/22)

Urgently fix the problem of not being able to automatically associate lyrics files.It is recommended that all users with version V2.76 update to this version.

## V2.76 (2023/04/02)

* Added the function of customizing the mini mode UI. Added 2 mini mode UI.
* Added the function of self-drawn playlist. Added the self-drawn playlist to "UI 4", "UI 6", "Groove Music Style", "Groove Music Style (Windows11)", and "default UI".
* Added the function of customizing the save location of the album cover.
* Added the support for verbatim lyrics in ksc format.
* Added the function of "Play After Current Song" in the playlist right-click menu.
* Added the FLAC format output support in Format conversion.
* Added the function to sync playing tracks and playback records to LastFM.
* Added support of the right-click menu by long-pressing on touchscreen devices.
* Added the function of saving the cue audio tracks to the media library.
* Added the function of customizing the row height of the playlist.
* Added the function of customizing the buttons in the self-drawn title bar.
* Added a dark/light toggle button.
* Added the function to display track information in the lyrics area when there are no lyrics.
* Updated several icons, fixed some icon that were blurry when scaled at 100%.
* Fixed the issue that the program cannot be exited by right-clicking the notification area icon when a dialog box opened.
* Added the function of writing the cue tag information and displaying the advanced label information of cue format.
* Fixed the issue that the playlist cannot be saved in m3u and m3u8 formats when the playlists contains cue tracks.
* When sorting by track number in the media library, sorting by CD number is preferred.
* Add thread synchronization when media library update, increase the stability of the program.
* Fixed the problem that conversion complite displayed when conversion errors in Format Conversion.
* Added the functions of "Fix the wrong file path" and "Open the playlist file location" in the right-click menu of the playlist in the media library.
* Added the function to remember the index of Stack Elements in the user defined UI.
* Added the option of enable or disable MediaTransControl in the Option Settings.
* Added "Artist - Album" type to Text element in user defined UI.
* Improvements in other details.
* Other bug fixes.

## V2.75 (2022/06/04)

* Add the FFMPEG playback core (need to be downloaded separately).
* Add some new UI layouts.
* The UI 3, UI 4 and UI 5 are reproduced using xml and optimized the effect of UI 4.
* Add stacking components to custom UI.
* Optimized the use logic of floating playlist.
* Fix the problem that buffer shortage may occur when writing ini.
* When creating a new playlist in the media library, the new playlist is brought to the front.
* Self-drawn controls follow the "Use Rounded Corner Style" setting.
* Fixed the problem that "Reverb" in "Sound Settings" could not be saved.
* Add the self-drawn menu bar.
* Improvements in other details.

## V2.74 (2022/02/12)

* Added the function of customizing the UI through the xml file (please refer to the main menu "Help" - "Customize UI").

* Added 3 new UI layout.
* Added support for System Media Transport Controls, which integrates playback controls into the system and supports the use of multimedia keys or Bluetooth devices to control music playback.
* Added the function to quickly switch folders/playlists through the drop-down menu of the "Folder/Playlist" text box.
* Added settings for the save location of configuration and data files.
* Added the volume icon.
* Added the display of sampling frequency, number of channels and bit depth in song properties.
* Added the function to delete the embedded album by "Delete Album Cover" in the context menu.
* Fixed a crash when "Allow play information displayed in the taskbar search box" is enabled under Windows 11.
* Added some icons to some dialogs.
* The floating playlist moves with the main window.
* Fixed an issue where song info would sometimes be refetched on every launch.
* Added theo ption "Do not display blank lines in single-line and double-line display mode" in "Options" - "Lyric Settings".

## V2.73 (2021/08/22)

* Added two new UI layout.

* Added the function of song rating.
* Added the function of manually refreshing the media library in the media library settings.
* Added the function of using self-drawn title bar (remove the check of "Main Menu-View-Use System Standard Title Bar").
* Added support for multi-monitor in mini mode.
* Absolute path is supported for external album cover in option settings.
* Added support for lyric files encoded in UTF16 format.
* Added the analysis of the original text and translation of the lyrics with the same time tag.
* Added the function of customizing the output file name in the conversion format.
* Added the setting to display or hide the playlist tool tip in the option settings.
* Added desktop lyrics alignment settings.
* The display of the desktop lyrics follow the settings of the lyrics karaoke style.
* Added "show/hide" player shortcut keys in global shortcut key settings.
* Added the option of "disable delete from disk" in the option settings.
* Added the function of automatically switching the playback device when the playback device changes.
* Added the setting of writing version of ID3v2.
* Fixed the problem that when using the mouse wheel to scroll the page in the option settings, the controls on the page will respond to the mouse wheel message.
* Fixed the problem that some Unicode characters in the written label information will become question marks after being converted to mp3 format in the conversion format.
* Fixed the problem that the global shortcut key settings may be wrong when loading from the configuration file.
* Fixed the problem that the last character of the desktop lyrics cannot be displayed when using certain fonts.
* Fixed an issue where the current playlist may be saved to the wrong file when switching playlists.
* Improvements in other details.

## V2.72 (2021/01/24)

* Add a new interface layout.
* Add the display of tracks in the playlist in "Media Library"-"Playlist" interface.
* Add the "Length" column to the list in the "Lyric download" dialog box.
* Add the support of the embedded cue tracks.
* Add the "Advanced tag info" tab in the "Property" dialog box.
* Add the function to use the desktop background as the background.
* Add the support for relative paths in playlist files.
* Add the function to jump to the current song artist and album in the media library in the right-click menu.
* Added support for TAK format audio (32-bit version only).
* The "Edit Lyrics" editor changed to use Scintilla. Add the colors for the lyric editor.
* Fixed the problem that the album cover cannot be copied to the target file when the format is converted to mp3 and wav.
* Fixed the problem that there is no icon in "Repeat Mode"-"Play Random" in the desktop lyrics right-click menu.
* Fixed the problem that the sub-dialog in the "Option Setting" dialog cannot be scrolled using the touch screen when using a touch screen device.
* Fixed the problem that "file already exists" is prompted when adding a currently playing track to a new playlist.
* Fixed the problem that if the position of the child window in the "Option Setting" dialog is scrolled by the mouse wheel, the position of the child window is incorrect when the scroll bar is clicked.
* Fixed the problem that the playlist does not refresh after renaming.
* Fixed the problem that the position of the progress bar would be incorrect when the rewind command was executed when the playback progress was less than 5 seconds.
* Fixed the problem that the taskbar search box information cannot be displayed after the list of tracks is played in repead mode of "Play in order", and fixed the problem that the taskbar search box information cannot be displayed when this function is turned on after turned off.
* Fixed the problem that the volume cannot be changed when the playlist is empty.

## V2.71 (2020/09/13)

* Use the taglib library to obtain audio tags and album cover. Add the tags and album cover acquisition of more than a dozen audio formats.
* Add the function to write audio tags and album cover.
* Add the function of batch editing of tags in the audio properties.
* Add the functions of "Get tags from file name", "Get tags from lyrics" and "Get tags online" in the audio properties.
* Add the "Album Cover" tab to the audio properties.
* Fixed the problem that the playlist was disordered in folder mode when adding songs to the playlist that was just opened.
* Add read and write functions for embedded lyrics.
* Add the file rename function.
* Fixed the problem that the temporary file cannot be renamed when the existing target file is selected to overwrite the existing target file in the format conversion.
* Improvements in other details.

## V2.70.1 (2020/08/27)

* Fixed the problem of the play random and play shuffle did not work properly when the number of tracks in the playlist is too large.

## V2.70 (2020/08/26)

* Fixed the problem of getting some FLAC audio tag information and album cover.
* Update the playback control icon.
* The application icon is redesigned.
* Add icons for menu items.
* Add the option of interface refresh interval in "Option Settings"-"Appearance Settings".
* Update the phonograph picture when the album cover is not displayed.
* Add the function of adding the currently playing track to the playlist.
* Fixed the problem that opening multiple audio files at the same time from the Explorer will start multiple processes to play simultaneously.
* Fixed the problem that UTF8 format cue files could not be read normally. Added support for UTF16-LE-BOM encoding format cue files.
* Fixed the problem that the playlist would freeze when loading if there are too many lyric files.
* Add the function of manually link the local lyrics.
* Add the function of changing the notification area icon.
* Add the rounded-corner style for the buttons.
* Fix the problem of handle leakage.
* Fixed the problem that in "Option Settings"-"Global Shortcut Keys", a certain shortcut key is set to "None", which causes other shortcut keys to become invalid.
* Fixed the issue of freezing when opening the "Media Library" dialog box.
* Fixed the problem that the length and bit rate of the corresponding audio were not obtained when opening a cue file from the media library.
* If the album cover size is too large, reduce it to solve the problem of UI freezing.
* Put the process of UI drawing in a background thread to solve the problem of message blocking caused by UI drawing take too much time.
* Fixed the problem that when downloading cue lyrics, if the artist or title contains characters that cannot be used as the file name, it cannot be downloaded.
* Fixed the problem that the id3v2 tag in wav format could not be obtained.
* Fixed the problem that the comma in the field cannot be processed correctly when exporting the csv file in "Play Time Statistics".
* Add the function of sort by file modification time in the sorting function of the playlist, as well as the functions of sorting in ascending and descending order.
* Fix the problem that the album cover in bmp format cannot be obtained.
* Added the function of including subfolders in the folder mode
* Fix the problem that when adding tracks from the media library to the current playlist, the current playlist is not refreshed.
* The "File Type" and "Bitrate" tabs are added to the media library (need to be turned on in "Option Settings"-"Media Library").
* Add the function of changing the default background picture ("Option Settings"-"Appearance Settings").
* Add "Play shuffle" mode to repeat mode (The original "Play shuffle" was actually "Paly Random").
* Added the function of auto run when Windows starts.
* Fixed an issue where playback cannot continue when the playback device changes.
* Slightly reduced the memory usage of the program.
* Fix some crash issues.

## V2.69 (2020/04/15)

* Add the AB repeat function.
* Add function to get embedded lyrics.
* Add the support for APE format tag recognition.
* Add the function of opening playlist, support m3u / m3u8 format.
* Fix the issue that when editing lyrics, the time label will be inserted into the end of the file when the cursor is placed at the front of the edited lyrics.
* Add the function of saving playlist as playlist file.
* Add the function to start in mini mode (Main Menu-Tools-Create Shortcut-Create Mini Mode Shortcut).
* Added the "Recently played" tab in the Media Library.
* Fixed an issue that there might be an extra character at the end when reading FLAC tags.
* Allow to enter mini mode in full screen mode.
* Sort the texts in the playlist and media library in the local language.
* Other bug fixes and improvements in details.

## V2.68 (2020/01/22)

* Add "Artist", "Album", "Genre" and other ways to organize songs in the media library. Add "All tracks" and "Folder explore" in the media library.
* Add the media library options.
* Add the function of automatically scanning audio files in the specified directory at startup.
* Add the function to convert the frequency in format convert.
* Add more functions in the right-click menu of the "Find" dialog.
* Fixed the problem that when converting to MP3 format if the file name contains Unicode characters, the conversion will be failed.
* Add the option of the file saved location when downloading lyrics automatically.
* Add the option of lyrics alignment.
* Some bug fixes and Improved in details.
## V2.67 (2019/11/28)
* Add the desktop lyric function.
* Add a playlist toolbar.
* The cue track can be added to playlist.
* Add the function to adjust playback speed.
* Fixed some bugs.
* Improved in other details.
## V2.66 (2019/08/03)
* Add the playlist function, you can freely create and edit playlists.
* Add the function of song red heart.
* Fixed the problem that some flac audio tags were incorrectly obtained.
* Add the function of file association.
* Fixed the problem that the GDI handle leaked when the window size changed.
* Add the sound fade effect.
* Fixed some bugs.
* Improved in other details.
## V2.65 (2019/05/25)
* Add the function to display spectrum analysis in the search box.
* Add the function of accumulating listening time statistics.
* Add the function of floating playlist.
* Add the function of always on top.
* Add the function to display error messages and generate a dump file when crashed.
* Add the style settings of bold, italic, and underline font settings.
* Add the function to create playback control shortcuts.
* Add the x64 version.
* Improvements in other details
## V2.64 (2019/02/16)
* Adjust the UI. Beautify the playback control buttons and progress bar.
* Added the function to hide/display playlists.
* Added the function to hide/display menu bar.
* Added the function to respond the multimedia keys.
* Optimize the display effect of the Cortana search box lyrics .
* Added the function of full-screen display.
* Other adjustments for the UI details.
* Improved in other details.
## V2.63 (2019/01/06)
* The main interface is redesigned to look more concise and beautiful. You can still switch back to the previous interface quickly.
* Mini Mode interface redesigned. Using the same style as the main interface.
* Optimize the function to get the theme color of the system. Get the actual system theme color instead of the window title bar color.
* Fix the problem that causes the program crash when running for a period of time due to a GDI handle leak.
* Add the global shortcut keys settings.
* Add the notification area icon. When you close the main window, you can make it minimize to the notification area instead of exit the program.
* Add the English language support.
* Improved in other details.
