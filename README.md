<h1>Virtual pet!</h1>

Tamagotchi fangame made in C++ with sfml libriary. All sprites by me and uleczal. 
<br>Take care of your virtual pet! Collect coins. 
<br>

![obraz](https://github.com/user-attachments/assets/032d0993-4d40-42ac-aec7-702cb9abe97b)
<br>Stats.<br>
![obraz](https://github.com/user-attachments/assets/359c4001-9cd1-45f6-b0ff-b0fb8b768cf4)
<br>Login page.<br>
![obraz](https://github.com/user-attachments/assets/b6dc2daa-42e9-41c4-891a-77770d92230d)
<br>Test your reflex in a minigame and earn coins!<br>
![obraz](https://github.com/user-attachments/assets/4da29b89-7aa6-4ebe-8197-1fab738ca3e9)
<br>Grocery store for spending all your precious coins. <br>

<b>Special thanks to uleczal - the co-author.</b>

<h1>Now on your machine (wip)</h1>
1) open projekt.sln <br>
2) copy folder sfml from mametchi folder to folder containing mametchi folder <br>
3) rename newly created folder SFML <br>
4) correct paths in project properties in C/C++ and Linker menus <br>
5) add to preprocesor definitions: SFML_DYNAMIC;_DEBUG;_CONSOLE; <br>
6) add to linker additional dependencies: sfml-graphics-d.lib;sfml-window-d.lib;sfml-audio-d.lib;sfml-network-d.lib;sfml-system-d.lib; <br>
7) copy from SFML/bin/ dlls to Debug/ <br>
8) clean build <br>
9) run in debug <br>
10) wait for symbols to load <br>

<h2>Known issuess</h2>
1) night sometimes never comes
2) buttons text change colours sometimes
3) buttons rectangles/background may disappear completely, but remain functional
