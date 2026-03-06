# main.cpp and win32 stuff

- `main.cpp:main` is where execution begins
  - `main` calls `wWinMain`

- `wWinMain` creates `App app` - the `app` object contains the entire application
  - `wWinMain` calls `makeWindow` and passes in `&app`
    - this sets `&app` as the window's "user data"
  - `wWinMain` enters the application's main loop:
    ```cpp
    MSG message = {};
    while (message.message != WM_QUIT) {
        if (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&message);
            DispatchMessageW(&message);
        } else {
            app.tick();
            Sleep(10);
        }
    }
    ```
    - `PeekMessageW` checks if there are any pending messages, i.e. keydown, keyup, mouse click
      - if there is a message, it handles it by calling `DispatchMessageW`
        - `DispatchMessageW` calls the window's message handler function. in this case this is `windowProc`
          - this is set in `makeWindow` by doing `windowClass.lpfnWndProc = windowProc`
      - if there is no message, it does `app.tick()` and `Sleep(10)`

- `windowProc`
  - this is the function that handles any messages
  - it takes inputs `UINT message`, `WPARAM wParam` and `LPARAM lParam`
    - `message` is the message type, i.e. keydown, keyup, mouse click
    - `WPARAM` is a `UINT_PTR`
    - `LPARAM` is a `LONG_PTR`
    - depending on the type of message, these can encode different things. for example, if the event is a mouse click event, these encode the mouse x and y position.
  - if `message == WM_CREATE`, the window is being created
    - in this case, the "user data", i.e. the pointer to `app`, is being passed in as `lParam`
    - we need to store it in the window's "instance data"
  - the `else` clause handles any events besides `WM_CREATE` and `WM_DESTROY`, i.e. mouse clicks, etc.
    - we get the `app` pointer
    - we call `app->shouldHandleMessage` to see if the app should handle the message
      - there are many messages that the app shouldn't handle, i.e. window resizing, etc.
      - we need to call `DefWindowProcW` to handle these
    - if the message is something `app` should handle, we call `app->handleMessage`

# app.hpp

- `app.hpp` contains `App`, the object that contains the entire application

- `App` has a member variable, `SharedData sharedData`
  - `sharedData` contains `rootUgen` and `ugenCtx`

- how the event loop works:
  - recall that `wWinMain` calls `app.tick` every 10 ms or so
  - `app.tick`
    - we call `ui.handleTick` which handles all UI logic that needs to happen during that tick
    - we then call `gfx.invalidateWindow`
      - this causes Windows to generate a `WM_PAINT` event sometime in the near future
      - `handleMessage` handles `WM_PAINT` by calling `onPaint`
  - `app.onPaint`
    - `gfx.beginDraw`
      - tell the graphics system that we're going to begin drawing to the back buffer
    - `ui.handleDraw`
      - walk the UI tree, each element pushes data to the draw queue
    - `gfx.render()`
      - this iterates over the draw queue and calls the actual draw functions, which write to the back buffer
    - `gfx.endDraw`
      - the back buffer becomes the front buffer, i.e. the pixel data becomes visible on the screen

















