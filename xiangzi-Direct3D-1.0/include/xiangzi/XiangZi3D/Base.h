
Vertex vertices[] =
{
{ { -0.5f, -0.5f, -0.5f }, XMFLOAT4(Colors::Red) },     // left-bottom-front
{ { -0.5f, 0.5f, -0.5f }, XMFLOAT4(Colors::Yellow) },   // left-up-front
{ { 0.5f, 0.5f, -0.5f }, XMFLOAT4(Colors::Green) },     // right-up-front
{ { 0.5f, -0.5f, -0.5f }, XMFLOAT4(Colors::Orange) },   // right-bottom-front
{ { -0.5f, -0.5f, 0.5f }, XMFLOAT4(Colors::Pink) },     // left-bottom-back
{ { -0.5f, 0.5f, 0.5f }, XMFLOAT4(Colors::Blue) },      // left-up-back
{ { 0.5f, 0.5f, 0.5f }, XMFLOAT4(Colors::Black) },      // right-up-back
{ { 0.5f, -0.5f, 0.5f }, XMFLOAT4(Colors::White) },     // right-bottom-back
};

int indices[] =
{
    // front face
    0, 1, 2,
    0, 2, 3,
    // back face
    4, 6, 5,
    4, 7, 6,
    // left face
    4, 5, 1,
    4, 1, 0,
    // right face
    3, 2, 6,
    3, 6, 7,
    // top face
    1, 5, 6,
    1, 6, 2,
    // bottom face
    4, 0, 3,
    4, 3, 7,
};
