**[简体中文](./update_log.md) | English**<br>

# MusicPlayer2 Update log
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
* Fixed the problem that there is no icon in "Repead Mode"-"Play Random" in the desktop lyrics right-click menu.
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
