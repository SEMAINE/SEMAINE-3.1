---------------
MOUSE FUNCTIONS
---------------

Through the buttons of the mouse it is possible to move the camera in the 3D space.
Left button: rotation around axis z 
Right button: zoom
Middle button: pan on the y



---------------------
LOAD DIFFERENT AGENTS
---------------------

You can choose among three agents: Greta, Girl and Sinkman.
In order to load a specific agent, open the greta.ini file and write the name of the agent in the last line of the file:
CHARACTER_SPEAKER=…

The possible names are: greta, girl, sinkman.

Example:
If you want to load Greta than, in the last line of the greta.ini file, write:

CHARACTER_SPEAKER=greta

save the file and run the FaceLibraryViewer.exe application.



-------------
SAVE GEOMETRY
-------------

In order to save the geometry of the agent, choose "Save geometry" into the File menu.
We suggest you to add the extension .vtx to the name of the file.



--------------------------------------
SAVE FACIAL EXPRESSION FOR THE LIBRARY
--------------------------------------

Click on the menu “File” and select “Save for face library (XML)”.
Tape the name of the new expression in the text field in the window “Save Faps for face library (XML)” that will pop up and click on the “Save” button.
In the same folder of the FaceLibrary application a file txt with the name of the expression will be saved.

The file is divided in 2 part: 

The fist one is called “DECLARATION” and contains the FAPs values for each fundamental part of the face.
Copy and Paste this part in the file “facelibrary.xml” in the “declaration” section.

The second part, called “IMPLEMENTATION”, contains the list of the facial elements involved in the new expression.
Copy and Paste this line in the file “facelibrary.xml” in the “implementation” section.

Reload the library to have access to the new expression from the FaceLibrary application.



------------------------------------
SAVE FACIAL EXPRESSION IN A FAP FILE
------------------------------------

Click on the menu “File” and select “Save FAP file”.
Tape the name of the new expression in the text field in the window “Save Fap File” that will pop up and click on the “Save” button.
In the same folder of the FaceLibrary application a file fap with the name of the expression will be saved. It is possible to visualize it on Greta using the GretaPlayer application.



------------------
RELOAD THE LIBRARY
------------------

Click on the menu “File” and select “Reload face library”.
