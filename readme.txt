Projektteilnehmer: Maximilian Jaesch

gameplay: Space um zu Springen. Erreichen Sie das Ziel um zu gewinnen.
_________________________________

Besonderer Fokus:
kollisionsdetektion: 
	(update methode der Applikation), es werden seperat die horizontale und vertikale bewegung abgearbeitet.

rotatePlayerModel und calcRotationSnapping: 
	der Spieler rotiert, wenn er fällt und wenn der Boden berührt wird, wird mithilfe der Werte von
	cosinus und sinus, die aus der Transform matrix ausgelesen werden, der Block mit dem Boden gleichgestellt.
	Desweiteren wird bei gedrückt gehaltener Jump taste die rotation nicht unterbrochen, wenn der Boden berührt wird.

ParticloSystem und ParticleShader: 
	die Verwendung von glDrawElementsInstanced um Rechenleistung zu sparen,
	das übergeben der Farb und Transformwerte der einzelnen Partikel in einem uniform float array, auf welches mithilfe von 
	gl_InstanceID zugegriffen wird.

________________________________

Alle Quellen: sind auch im Quelltext an der verwendeten Stelle angegeben.

https://stackoverflow.com/questions/4353525/floating-point-linear-interpolation - lerp für die farben
https://www.youtube.com/watch?v=GK0jHlv3e3w&t=222s -  the cherno particle system
https://learnopengl.com/Advanced-OpenGL/Instancing - instancing, für die partikel
https://stackoverflow.com/questions/5289613/generate-random-float-between-two-floats - random generation für partikelpositionen und bewegungen
https://docs.gl/gl4/glBindImageTexture - zum nachgucken der opengl dokumentation
https://learnopengl.com/Advanced-Lighting/Bloom - wurde im Endprojekt nicht umgesetzt

https://www.youtube.com/watch?v=h2r3_KjChf4 - jump quelle 1
https://www.youtube.com/watch?v=hG9SzQxaCm8 - jump quelle 2

Bildquellen:

https://www.freepik.com/free-vector/pink-slime-dripping-liquid-goo-mucus-texture_25581803.htm#query=poisonous%20texture&position=12&from_view=keyword
-> für die spikes
Gras Block textur: die seite scheint nicht mehr zu existieren. Spiel: Minecraft
https://learnopengl.com/img/textures/wood.png - holzplatten