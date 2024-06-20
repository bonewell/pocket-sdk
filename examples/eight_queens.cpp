#include <cmath>
#include <algorithm>

#include "bwgui/application.h"

struct Spot
{
	bool taken{false};
	int hit{0};
	operator bool() const { return taken; }
};

template <size_t N>
using Spots = std::array<std::array<Spot, N>, N>;

template <size_t N>
bool is_row_taken(Spots<N> const &spots, int i, int j)
{
	for (auto k = 0; k < N; ++k)
	{
		if (k != j && spots[i][k])
			return true;
	}
	return false;
}

template <size_t N>
bool is_col_taken(Spots<N> const &spots, int i, int j)
{
	for (auto k = 0; k < N; ++k)
	{
		if (k != i && spots[k][j])
			return true;
	}
	return false;
}

template <size_t N>
bool is_left_diag_taken(Spots<N> const &spots, int i, int j)
{
	auto min = -std::min(i, j);
	auto max = std::min(int(N) - i - 1, int(N) - j - 1);
	for (auto k = min; k <= max; ++k)
	{
		if (k != 0 && spots[i + k][j + k])
			return true;
	}
	return false;
}

template <size_t N>
bool is_right_diag_taken(Spots<N> const &spots, int i, int j)
{
	auto min = -std::min(i, int(N) - j - 1);
	auto max = std::min(int(N) - i - 1, j);
	for (auto k = min; k <= max; ++k)
	{
		if (k != 0 && spots[i + k][j - k])
			return true;
	}
	return false;
}

template <size_t N>
bool is_legal(Spots<N> const &spots)
{
	for (auto i = 0; i < N; ++i)
	{
		for (auto j = 0; j < N; ++j)
		{
			if (spots[i][j])
			{
				if (is_row_taken(spots, i, j))
					return false;
				if (is_col_taken(spots, i, j))
					return false;
				if (is_left_diag_taken(spots, i, j))
					return false;
				if (is_right_diag_taken(spots, i, j))
					return false;
			}
		}
	}
	return true;
}

template <size_t N>
void take_spot(Spots<N> &spots, int i, int j)
{
	spots[i][j].taken = true;
	for (auto k = 0; k < N; ++k)
		++spots[i][k].hit;
	for (auto k = 0; k < N; ++k)
		++spots[k][j].hit;
	int min = -std::min(i, j);
	int max = std::min(int(N) - i - 1, int(N) - j - 1);
	for (int k = min; k <= max; ++k)
		++spots[i + k][j + k].hit;
	min = -std::min(i, int(N) - j - 1);
	max = std::min(int(N) - i - 1, j);
	for (int k = min; k <= max; ++k)
		++spots[i + k][j - k].hit;
}

template <size_t N>
void free_spot(Spots<N> &spots, int i, int j)
{
	spots[i][j].taken = false;
	for (auto k = 0; k < N; ++k)
		--spots[i][k].hit;
	for (auto k = 0; k < N; ++k)
		--spots[k][j].hit;
	int min = -std::min(i, j);
	int max = std::min(int(N) - i - 1, int(N) - j - 1);
	for (int k = min; k <= max; ++k)
		--spots[i + k][j + k].hit;
	min = -std::min(i, int(N) - j - 1);
	max = std::min(int(N) - i - 1, j);
	for (int k = min; k <= max; ++k)
		--spots[i + k][j - k].hit;
}

template <size_t N>
bool arrange(int count, Spots<N> &spots)
{
	if (!is_legal(spots))
		return false;
	if (count == N)
		return true;
	for (auto j = 0; j < N; ++j)
	{
		if (!spots[count][j] && spots[count][j].hit == 0)
		{
			take_spot(spots, count, j);
			if (arrange(count + 1, spots))
			{
				return true;
			}
			free_spot(spots, count, j);
		}
	}
	return false;
}

struct Rectangle : bwgui::Rectangle<int>
{
	bwgui::Color c;
};

template <size_t N>
using Board = std::array<std::array<Rectangle, N>, N>;

struct Queen
{
	int i{0};
	int j{0};
};

template <size_t N>
class EightQueens : public bwgui::Application
{
public:
	EightQueens() : bwgui::Application("sierpinski_carpet")
	{
		CreateBoard();
	}

	void OnLoop() override;
	void OnRender() override;

private:
	void CreateBoard();
	void DrawRect(Rectangle const &rect) const;
	void DrawBoard() const;
	void DrawQueens() const;

	Board<N> board_;
	std::array<Queen, N> queens_;
	bool has_solution_{false};
};

template <size_t N>
void EightQueens<N>::CreateBoard()
{
	constexpr int begin_x = 10;
	constexpr bwgui::Size<int> s = {100, 100};
	bwgui::Color begin_c = bwgui::White;
	bwgui::Point<int> p = {begin_x, 10};
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
void EightQueens<N>::DrawRect(Rectangle const &rect) const
{
	bwgui::SetBrushColor(renderer(), rect.c);
	bwgui::FillRectangle(renderer(), rect);
	bwgui::SetBrushColor(renderer(), bwgui::Gray);
	bwgui::DrawRectangle(renderer(), rect);
}

template <size_t N>
void EightQueens<N>::DrawBoard() const
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
void EightQueens<N>::DrawQueens() const
{
	if (!has_solution_)
		return;
	for (auto const &q : queens_)
	{
		auto c = board_[q.i][q.j];
		bwgui::Point<int> p{c.p.x + 25, c.p.y + 25};
		bwgui::Size<int> s{c.s.w - 50, c.s.h - 50};
		Rectangle r{p, s, bwgui::Red};
		DrawRect(r);
	}
}

template <size_t N>
void EightQueens<N>::OnRender()
{
	DrawBoard();
	DrawQueens();
}

template <size_t N>
void EightQueens<N>::OnLoop()
{
	Spots<N> spots;
	has_solution_ = arrange(0, spots);
	auto queen = std::begin(queens_);
	for (auto i = 0; i < N; ++i)
	{
		for (auto j = 0; j < N; ++j)
		{
			if (spots[i][j])
			{
				*queen = {i, j};
				++queen;
			}
		}
	}
}

int main()
{
	EightQueens<8> app;

	return app.Execute();
}
