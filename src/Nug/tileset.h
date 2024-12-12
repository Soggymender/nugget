class Tileset
{

public:
    Tileset() { }

    bool Create(const char* filename);
    void Destroy();

    class Tile
    {
    public:
        float u;
        float v;
    
        // Flags.
    };

    Tile* tiles; 

    float textureTileWidth; 
    float textureTileHeight;
};