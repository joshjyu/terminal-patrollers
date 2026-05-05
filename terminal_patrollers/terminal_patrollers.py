import curses
import random


def _get_centered_coords(
    stdScreen: "curses.window", textLen: int, offsetY: int = 0
) -> tuple[int, int]:
    """
    Calculates the Y and X coordinates to center text on the screen.

    Parameters:
      stdScreen ("curses.window"): The window object.
      textLen (int): Length of the text to be centered.
      offsetY (int): Vertical offset from the absolute center.
    Returns:
      tuple[int, int]: The calculated (y, x) coordinates.
    """
    maxY, maxX = stdScreen.getmaxyx()
    x = (maxX - textLen) // 2
    y = (maxY // 2) + offsetY

    return y, x


def _run_confirm_exit(stdScreen: "curses.window") -> bool:
    """
    Prompts the user to confirm exiting via an interactive menu.

    Parameters:
      stdScreen ("curses.window"): The standard screen object.
    Returns:
      bool: True if the user confirms exit, False otherwise.
    """
    options = ["YES, QUIT TO THE TERMINAL", "NO, RETURN TO THE MENU"]
    currentSelection = 1  # Default to "No" for safety

    while True:
        stdScreen.clear()

        warnTitle = "WARNING: EXIT GAME?"
        warnDesc = "Are you sure you want to quit?"

        # Add exit confirmation text
        y, x = _get_centered_coords(stdScreen, len(warnTitle), -2)
        stdScreen.addstr(y, x, warnTitle, curses.A_BOLD)
        y, x = _get_centered_coords(stdScreen, len(warnDesc), -1)
        stdScreen.addstr(y, x, warnDesc)

        # Render options and highlight selections
        for i, option in enumerate(options):
            label = f"> {option}" if i == currentSelection else f"  {option}"
            y, x = _get_centered_coords(stdScreen, len(label), 1 + i)

            if i == currentSelection:
                stdScreen.attron(curses.A_REVERSE)
                stdScreen.addstr(y, x, label)
                stdScreen.attroff(curses.A_REVERSE)
            else:
                stdScreen.addstr(y, x, label)

        stdScreen.refresh()
        keyPressed = stdScreen.getch()

        # Handle WASD and Arrow Keys for vertical navigation
        if keyPressed in (curses.KEY_UP, ord("w"), ord("W")):
            currentSelection = max(0, currentSelection - 1)
        elif keyPressed in (curses.KEY_DOWN, ord("s"), ord("S")):
            currentSelection = min(len(options) - 1, currentSelection + 1)

        # Handle ENTER key (10 and 13 cover various terminal environments)
        elif keyPressed in (curses.KEY_ENTER, 10, 13):
            return currentSelection == 0

        # Handle terminal window resizing
        elif keyPressed == curses.KEY_RESIZE:
            curses.update_lines_cols()


def _run_main_menu(stdScreen: "curses.window") -> int:
    """
    Draws and handles the interactive main menu.

    Parameters:
      stdScreen ("curses.window"): The standard screen object.
    Returns:
      int: The index of the selected option (0 for Play, 1 for Quit).
    """
    options = ["PLAY", "QUIT"]
    currentSelection = 0

    while True:
        stdScreen.clear()

        title = "TERMINAL PATROLLERS"
        desc = "A fun stealth game mapped directly to real-world city streets!"
        costNote = "NOTE: Requires an active microservice network connection."

        # Add title
        y, x = _get_centered_coords(stdScreen, len(title), -4)
        stdScreen.addstr(y, x, title, curses.A_BOLD)

        # Add description
        y, x = _get_centered_coords(stdScreen, len(desc), -2)
        stdScreen.addstr(y, x, desc)

        # Add cost note
        y, x = _get_centered_coords(stdScreen, len(costNote), 0)
        stdScreen.addstr(y, x, costNote, curses.A_DIM)

        # Render options and highlight selections
        for i, option in enumerate(options):
            label = f"> {option}" if i == currentSelection else f"  {option}"
            y, x = _get_centered_coords(stdScreen, len(label), 3 + i)

            if i == currentSelection:
                stdScreen.attron(curses.A_REVERSE)
                stdScreen.addstr(y, x, label)
                stdScreen.attroff(curses.A_REVERSE)
            else:
                stdScreen.addstr(y, x, label)

        stdScreen.refresh()
        keyPressed = stdScreen.getch()

        # Handle WASD and Arrow Keys for vertical navigation
        if keyPressed in (curses.KEY_UP, ord("w"), ord("W")):
            currentSelection = max(0, currentSelection - 1)
        elif keyPressed in (curses.KEY_DOWN, ord("s"), ord("S")):
            currentSelection = min(len(options) - 1, currentSelection + 1)

        # Handle ENTER key (10 and 13 cover various terminal environments)
        elif keyPressed in (curses.KEY_ENTER, 10, 13):
            return currentSelection

        # Handle terminal window resizing
        elif keyPressed == curses.KEY_RESIZE:
            curses.update_lines_cols()


def _is_valid_move(mapGrid: list[list[str]], targetY: int, targetX: int) -> bool:
    """
    Checks if the target coordinate is a valid movement space.

    Parameters:
      mapGrid (list[list[str]]): The map matrix.
      targetY (int): Target row index.
      targetX (int): Target column index.
    Returns:
      bool: True if the move is valid, False otherwise.
    """
    maxY = len(mapGrid)
    maxX = len(mapGrid[0]) if maxY > 0 else 0

    # Compress conditionals for bounds and wall collisions
    inBounds = 0 <= targetY < maxY and 0 <= targetX < maxX

    return inBounds and mapGrid[targetY][targetX] != "#"


def _calculate_new_pos(
    keyPressed: int, currentY: int, currentX: int
) -> tuple[int, int]:
    """
    Calculates the new coordinates based on the movement key pressed.

    Parameters:
      keyPressed (int): The ASCII or curses key code.
      currentY (int): The current row coordinate.
      currentX (int): The current column coordinate.
    Returns:
      tuple[int, int]: The potentially updated (Y, X) coordinates.
    """
    newY = currentY
    newX = currentX

    # W or Arrow Up
    if keyPressed in (curses.KEY_UP, ord("w"), ord("W")):
        newY -= 1
    # S or Arrow Down
    elif keyPressed in (curses.KEY_DOWN, ord("s"), ord("S")):
        newY += 1
    # A or Arrow Left
    elif keyPressed in (curses.KEY_LEFT, ord("a"), ord("A")):
        newX -= 1
    # D or Arrow Right
    elif keyPressed in (curses.KEY_RIGHT, ord("d"), ord("D")):
        newX += 1

    return newY, newX


def _generate_patrollers(
    mapGrid: list[list[str]], numPatrollers: int
) -> list[tuple[int, int]]:
    """
    Generates a list of valid starting coordinates for patrollers.

    Parameters:
      mapGrid (list[list[str]]): The map matrix.
      numPatrollers (int): The number of patrollers to spawn.
    Returns:
      list[tuple[int, int]]: A list of (Y, X) coordinate tuples.
    """
    # Initialize list of patrollers
    patrollers = []
    maxY = len(mapGrid)
    maxX = len(mapGrid[0]) if maxY > 0 else 0

    while len(patrollers) < numPatrollers:
        # Random generation is a placeholder until improved game design
        randY = random.randint(0, maxY - 1)
        randX = random.randint(0, maxX - 1)

        # Compress conditionals to ensure patrollers spawn on empty streets
        isEmpty = mapGrid[randY][randX] == "."
        isStart = randY == 0 and randX == 0

        if isEmpty and not isStart and (randY, randX) not in patrollers:
            patrollers.append((randY, randX))

    return patrollers


def _draw_hud(
    stdScreen: "curses.window",
    mapGrid: list[list[str]],
    playerY: int,
    playerX: int,
    numPatrollers: int,
):
    """
    Renders the heads-up display (HUD) below the active game map.

    Parameters:
      stdScreen ("curses.window"): The standard screen object.
      mapGrid (list[list[str]]): The 2D array of map characters.
      playerY (int): The player's current row coordinate.
      playerX (int): The player's current column coordinate.
      numPatrollers (int): The number of active patrollers.
    Returns:
      None
    """
    mapHeight = len(mapGrid)

    # HUD's minimal necessary information
    coordsText = f"COORDS: {playerX}, {playerY}"
    enemyText = f"PATROLLERS ACTIVE: {numPatrollers}"

    # Add coordinate HUD text
    y, x = _get_centered_coords(stdScreen, len(coordsText), (mapHeight // 2) + 2)
    stdScreen.addstr(y, x, coordsText)

    # Add patroller count HUD text
    y, x = _get_centered_coords(stdScreen, len(enemyText), (mapHeight // 2) + 3)
    stdScreen.addstr(y, x, enemyText)

    # Backtracking and pause options
    instructTxt = "[R] Restart Run  |  [ESC] Pause Menu"
    y, x = _get_centered_coords(stdScreen, len(instructTxt), (mapHeight // 2) + 5)
    stdScreen.addstr(y, x, instructTxt, curses.A_DIM)


def _load_fake_map() -> list[list[str]]:
    """
    Generates a static 2D grid representing a city map.
    This is intended to be a placeholder map.

    Parameters:
      None
    Returns:
      list[list[str]]: A 2D list of map characters.
    """
    rawMap = [
        "...........................",
        "....#######.....#######....",
        "....#######.....#######....",
        "...........................",
        "....#######................",
        "....#######.......#####....",
        "..................#####....",
        "...........................",
    ]

    return [list(row) for row in rawMap]


def _draw_map(
    stdScreen: "curses.window",
    mapGrid: list[list[str]],
    playerY: int,
    playerX: int,
    patrollers: list[tuple[int, int]],
):
    """
    Iterates through the map matrix and renders it on screen.

    Parameters:
      stdScreen ("curses.window"): The standard screen object.
      mapGrid (list[list[str]]): The 2D array of map characters.
      playerY (int): The player's row coordinate.
      playerX (int): The player's column coordinate.
      patrollers (list[tuple[int, int]]): List of patroller coordinates.
    Returns:
      None
    """
    mapHeight = len(mapGrid)
    mapWidth = len(mapGrid[0]) if mapHeight > 0 else 0

    startY, startX = _get_centered_coords(stdScreen, mapWidth, -(mapHeight // 2))

    for rIndex, row in enumerate(mapGrid):
        for cIndex, char in enumerate(row):
            isPlayer = rIndex == playerY and cIndex == playerX
            isPatroller = (rIndex, cIndex) in patrollers

            # Prioritize rendering the player over a patroller if they overlap
            if isPlayer:
                drawChar = "@"
            elif isPatroller:
                drawChar = "P"
            else:
                drawChar = char

            stdScreen.addstr(startY + rIndex, startX + cIndex, drawChar)


def _run_game_loop(stdScreen: "curses.window"):
    """
    Handles the active game view state and map rendering.

    Parameters:
      stdScreen ("curses.window"): The standard screen object.
    Returns:
      None
    """
    mapGrid = _load_fake_map()

    # Player starts at top left of map
    playerY = 0
    playerX = 0

    # Generates 3 patrollers
    patrollerCount = 3
    patrollers = _generate_patrollers(mapGrid, patrollerCount)

    while True:
        stdScreen.clear()

        _draw_map(stdScreen, mapGrid, playerY, playerX, patrollers)
        _draw_hud(stdScreen, mapGrid, playerY, playerX, len(patrollers))

        stdScreen.refresh()
        keyPressed = stdScreen.getch()

        # [ESC] to Quit (Placeholder for Pause Menu)
        if keyPressed == 27:  # 27 is standard ASCII code for Escape key
            break
        # [R] to Restart
        elif keyPressed in (ord("r"), ord("R")):
            playerY = 0
            playerX = 0
            patrollers = _generate_patrollers(mapGrid, patrollerCount)
        elif keyPressed == curses.KEY_RESIZE:
            curses.update_lines_cols()
        else:
            # Calculate and validate player movement
            newY, newX = _calculate_new_pos(keyPressed, playerY, playerX)
            if _is_valid_move(mapGrid, newY, newX):
                playerY = newY
                playerX = newX


def main(stdScreen: "curses.window"):
    """
    Main application loop handling routing and initialization.

    Parameters:
      stdScreen ("curses.window"): The standard screen object.
    Returns:
      None
    """
    curses.curs_set(0)  # Hide cursor
    stdScreen.keypad(True)  # Enable escape sequences

    while True:
        # Enter main menu
        selectionIndex = _run_main_menu(stdScreen)

        # 0 for PLAY
        if selectionIndex == 0:
            _run_game_loop(stdScreen)
        # 1 for QUIT
        elif selectionIndex == 1:
            if _run_confirm_exit(stdScreen):
                break


if __name__ == "__main__":
    curses.wrapper(main)
