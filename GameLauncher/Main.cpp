#include <Siv3D.hpp>


class SnapShot
{
private:
    enum class State
    {
        Moving,
        Moved,
        Rest
    };
private:
    State m_state;
    std::vector<Vec2> m_vertexPositions;
    std::vector<EasingController<Vec2>> m_vertexControllers;
public:
    SnapShot() = default;
    SnapShot(std::vector<Vec2> vertexes)
        : m_state(State::Rest)
        , m_vertexPositions(vertexes)
        , m_vertexControllers(vertexes.size())
    {
    }
    ~SnapShot() = default;
public:
    void setEasing(std::vector<Vec2> nextVertexPositions)
    {
        if(m_state != State::Moving) {
            for(int i = 0; i < nextVertexPositions.size(); ++i) {
                m_vertexControllers[i] = EasingController<Vec2>(m_vertexPositions[i], nextVertexPositions[i], Easing::Sine, 300);
            }
            m_state = State::Moving;
            std::for_each(m_vertexControllers.begin(), m_vertexControllers.end(), [](EasingController<Vec2>& ec) {
                ec.start();
            });
        }
    }
    bool isMoved()
    {
        return m_state == State::Moved;
    }
    void update()
    {
        switch(m_state) {
            case State::Moving:
                for(int i = 0; i < m_vertexPositions.size(); ++i) {
                    m_vertexPositions[i] = m_vertexControllers[i].easeOut();
                }
                if(m_vertexControllers[0].isEnd())  m_state = State::Moved;
                break;
            default:
                break;
        }
    }
    void draw()
    {
        Quad(
            m_vertexPositions[0],
            m_vertexPositions[1],
            m_vertexPositions[2],
            m_vertexPositions[3]
        )(TextureAsset(L"TestSnapShot")).draw(Palette::White);
    }
};

std::vector<std::vector<Vec2>> positions = {
    {{-190, 80}, {-40, 120}, {-40, 380}, {-190, 420}},
    {{-30, 80}, {120, 120}, {120, 380}, {-30, 420}},
    {{130, 80}, {280, 120}, {280, 380}, {130, 420}},
    {{330, 100}, {630, 100}, {630, 400}, {330, 400}},
    {{680, 120}, {830, 80}, {830, 420}, {680, 380}},
    {{840, 120}, {990, 80}, {990, 420}, {840, 380}},
    {{1000, 120}, {1150, 80}, {1150, 420}, {1000, 380}}
};

std::vector<SnapShot> selects(positions.size());

void Main()
{
    Window::Resize({960, 720});
    TextureAsset::Register(L"TestSnapShot", L"Test.png");

    for(int i = 0; i < positions.size(); ++i) {
        selects[i] = SnapShot(positions[i]);
    }

    while(System::Update()) {
        if(selects[1].isMoved()) {
            for(int i = 0; i < positions.size(); ++i) {
                selects[i] = SnapShot(positions[i]);
            }
        }

        if(Input::KeyRight.pressed) {
            for(int i = 1; i < positions.size(); ++i) {
                selects[i].setEasing(positions[i - 1]);
            }
        }
        if(Input::KeyLeft.pressed) {
            for(int i = 0; i < positions.size() - 1; ++i) {
                selects[i].setEasing(positions[i + 1]);
            }
        }
        
        std::for_each(selects.begin(), selects.end(), [](SnapShot &select) {
            select.update();
        });
        std::for_each(selects.begin(), selects.end(), [](SnapShot &select) {
            select.draw();
        });
	}
}
