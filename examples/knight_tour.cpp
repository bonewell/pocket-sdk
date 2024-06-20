#include <array>
#include <cmath>
#include <optional>
#include <map>
#include <vector>

#include "bwgui/application.h"

using Spot = int;

template <size_t N>
using Spots = std::array<std::array<Spot, N>, N>;

template <size_t N>
std::ostream &operator<<(std::ostream &out, Spots<N> const &spots)
{
	for (int i = 0; i < N; ++i)
	{
		for (int j = 0; j < N; ++j)
			out << spots[i][j] << ',';
		out << '\n';
	}
	return out;
}

struct Turn
{
	int row{0};
	int col{0};
};
using Turns = std::vector<Turn>;
template <size_t N>
using LegalTurns = std::array<std::array<Turns, N>, N>;

template <size_t N>
LegalTurns<N> legal_turns()
{
	constexpr std::array<Turn, 8> turns = {
		Turn{-2, -1}, Turn{-2, 1}, Turn{-1, -2}, Turn{-1, 2},
		Turn{2, -1}, Turn{2, 1}, Turn{1, -2}, Turn{1, 2}};

	LegalTurns<N> legalturns;
	for (auto i = 0; i < N; ++i)
	{
		for (auto j = 0; j < N; ++j)
		{
			for (auto k = 0; k < 8; ++k)
			{
				auto r = i + turns[k].row;
				auto c = j + turns[k].col;
				if (r >= 0 && r < N && c >= 0 && c < N)
				{
					legalturns[i][j].push_back({r, c});
				}
			}
		}
	}

	return legalturns;
}

template <size_t N>
Turns get_turns(int r, int c)
{
	static auto turns = legal_turns<N>();
	return turns[r][c];
}

template <size_t N>
int get_options(Spots<N> const &spots, int r, int c)
{
	int count = 0;
	for (auto const &[i, j] : get_turns<N>(r, c))
	{
		if (spots[i][j] == 0)
			++count;
	}
	return count;
}

template <size_t N>
bool find_tour(int count, Spots<N> &spots, int r, int c)
{
	count++;
	spots[r][c] = count;
	if (count == N * N)
		return true;

	std::multimap<int, Turn> next_turns;
	for (auto const &t : get_turns<N>(r, c))
	{
		if (spots[t.row][t.col] == 0)
		{
			auto options = get_options(spots, t.row, t.col);
			next_turns.emplace(options, t);
		}
	}
	for (auto const &[k, t] : next_turns)
	{
		if (find_tour(count, spots, t.row, t.col))
			return true;
	}
	spots[r][c] = 0;
	return false;
}

using Point = bwgui::Point<int>;
using Size = bwgui::Size<int>;

struct Rectangle : bwgui::Rectangle<int>
{
	bwgui::Color c;
};

template <size_t N>
using Board = std::array<std::array<Rectangle, N>, N>;

struct Knight
{
	int i{0};
	int j{0};
};
template <size_t N>
using Tour = std::array<Knight, N>;

template <size_t N>
class KnightTour : public bwgui::Application
{
public:
	KnightTour() : Application("knight_tour", 200)
	{
		CreateBoard();
		Calculate();
	}

	void OnLoop() override;
	void OnRender() override;

private:
	void CreateBoard();
	void DrawRect(Rectangle const &rect) const;
	void DrawBoard() const;
	void DrawKnightTour() const;
	void Calculate();

	Board<N> board_;
	bool has_solution_{false};
	Tour<N * N> tour_;
	std::optional<int> current_;
};

template <size_t N>
void KnightTour<N>::CreateBoard()
{
	constexpr int begin_x = 10;
	constexpr Size s = {100, 100};
	bwgui::Color begin_c = bwgui::White;
	Point p = {begin_x, 10};
	for (auto &row : board_)
	{
		bwgui::Color color = begin_c;
		p.x = begin_x;
		for (auto &c : row)
		{
			c = {p, s, color};
			color = color == bwgui::White ? bwgui::Black : bwgui::White;
			p.x += s.w;
		}
		begin_c = begin_c == bwgui::White ? bwgui::Black : bwgui::White;
		p.y += s.h;
	}
}

template <size_t N>
void KnightTour<N>::DrawRect(Rectangle const &rect) const
{
	bwgui::SetBrushColor(renderer(), rect.c);
	bwgui::FillRectangle(renderer(), rect);
	bwgui::SetBrushColor(renderer(), bwgui::Gray);
	bwgui::DrawRectangle(renderer(), rect);
}

template <size_t N>
void KnightTour<N>::DrawBoard() const
{
	for (auto const &row : board_)
	{
		for (auto const &c : row)
		{
			DrawRect(c);
		}
	}
}

template <size_t N>
void KnightTour<N>::DrawKnightTour() const
{
	if (*current_ < N * N)
	{
		auto t = tour_[*current_];
		auto n = board_[t.i][t.j];
		Point p{n.p.x + 25, n.p.y + 25};
		Size s{n.s.w - 50, n.s.h - 50};
		Rectangle r{p, s, bwgui::Red};
		DrawRect(r);
	}
	for (int i = 0; i < *current_; ++i)
	{
		auto c = board_[tour_[i].i][tour_[i].j];
		Point p{c.p.x + 25, c.p.y + 25};
		Size s{c.s.w - 50, c.s.h - 50};
		Rectangle r{p, s, bwgui::Blue};
		DrawRect(r);
	}
}

template <size_t N>
void KnightTour<N>::OnRender()
{
	DrawBoard();
	DrawKnightTour();
}

template <size_t N>
void KnightTour<N>::Calculate()
{
	Knight start{0, 0};
	Spots<N> spots{0};
	has_solution_ = find_tour(0, spots, start.i, start.j);
	for (auto i = 0; i < N; ++i)
	{
		for (auto j = 0; j < N; ++j)
		{
			tour_[spots[i][j] - 1] = {i, j};
		}
	}
	if (!has_solution_)
	{
		tour_[0] = {start.i, start.j};
	}
}

template <size_t N>
void KnightTour<N>::OnLoop()
{
	if (current_)
	{
		if (has_solution_ && *current_ < N * N)
			++*current_;
	}
	else
		current_ = 0;
}

int main()
{
	KnightTour<8> app;

	return app.Loop();
}
