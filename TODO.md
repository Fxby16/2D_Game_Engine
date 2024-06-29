General:
- clean up the code since it has become very messy
Engine:
- remove ghost collisions on flat surfaces
- buttons
- rotated quads (and triangles)
- directional light
- particle system
--------------------------------------------------------------------------------------------------------------------
Editor:
- project export
- tilemap editor
- camera (zoom)

- undo/redo system
--------------------------------------------------------------------------------------------------------------------
Others:
- windows porting




StaticColliderComponent

4 b2EdgeShape
uint8_t shapes_enabled: DOWN = 1 (1<<0)
                        UP = 2 (1<<1)
                        LEFT = 4 (1<<2)
                        RIGHT = 8 (1<<3)

Can only be attached to a static body

Every shape must be in the same group.
Every shape must have the same category and mask bits.

StaticColliderComponent
    -Shape 1
        X: xxxx Y: xxxx
        XOffset: xxxx YOffset: xxxx
        -Ghost Vertex Left
            X: xxxx Y: xxxx
            XOffset: xxxx YOffset: xxxx
        -Ghost Vertex Right
            X: xxxx Y: xxxx
            XOffset: xxxx YOffset: xxxx
    -Shape 2
        ...
    -Shape 3
        ...
    -Shape 4
        ...