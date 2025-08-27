# Hotel Game (C++ / SFML)

![Gameplay Demo](demo.gif)

## 🏨 **Hotel Desk Game**
An immersive C++/SFML simulation where players step into the role of a desk clerk in an eerie hotel and detect anomalies in guests checking in.

### FULL DEMO LINK: [https://youtu.be/Y1LotFzByAA]

### 🧪 Game Rules & Anomalies
For each arriving claimer, you see:
a badge (guestID, name, portraitID), and
the live person.
You must APPROVE only clean badges, and FLAG anything suspicious.


A badge is considered clean only if all are true:

**ID Format:** The guest's ID must be exactly 8 characters and all digits (0–9).

**Today’s List:** The guest's ID must be in today’s expected guest list (displayable with TAB).

**Portrait Match:** The portrait on the guest's ID must EXACTLY match the person at the desk.
