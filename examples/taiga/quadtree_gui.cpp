#include "bwgui/action.h"
#include "bwgui/application.h"
#include "bwgui/point_generator.h"

#include "taiga/quad_tree.h"

struct Item
{
	bwgui::Circle<int> circle;
	bwgui::Color color{bwgui::Black};

	auto& position() const { return circle.c; };
	bool inside(taiga::Point<int> const& p) const
	{
		auto dx = std::abs(circle.c.x - p.x);
		auto dy = std::abs(circle.c.y - p.y);
		return (dx * dx + dy * dy) <= (circle.r * circle.r);
	}
};

auto generate_objects(bwgui::Size<int> s, int n)
{
	bwgui::Point<int> min{0, 0};
	bwgui::Point<int> max{s.w, s.h};

	auto points = bwgui::generate_points(min, max, n);
	std::vector<Item> items{};
	items.reserve(n);
	for (auto const& p : points)
	{
		items.push_back({{p, 50}});
	}
	return items;
}

class QuadTreeGui: public bwgui::Application
{
public:
	QuadTreeGui();

	void OnClick(bwgui::Point<int> p) override;
	void OnLoop() override;
	void OnRender() override;
	
private:
	taiga::QuadTree<int, Item> tree_;
	std::optional<taiga::Point<int>> location_;
	Item* found_;
};

QuadTreeGui::QuadTreeGui() : Application("quadtree_gui", 100)
{
	auto size = GetSize();
	taiga::Area<int> area{{0, 0}, {size.w, size.h}};
	tree_ = taiga::make_quad<int, Item>(area);
	auto objects = generate_objects(size, 50);
	for (auto o: objects) taiga::add_item(tree_, std::move(o));
}

void QuadTreeGui::OnClick(bwgui::Point<int> p)
{
	location_ = {p.x, p.y};
}

void QuadTreeGui::OnLoop()
{
	if (!location_) return;

	if (found_) found_->color = bwgui::Black;
	found_ = taiga::find_item(tree_, *location_);
	if (found_) found_->color = bwgui::Red;
	location_ = std::nullopt;
}

void QuadTreeGui::OnRender()
{
	if (!tree_) return;
	auto grid = [this](taiga::QuadNode<int, Item> const& node)
	{
		bwgui::SetBrushColor(renderer(), bwgui::Pebble);
		bwgui::DrawRectangle<int>(renderer(), {
			{node.area.p.x, node.area.p.y},
			{node.area.s.w, node.area.s.h}});
	};
	taiga::traverse_nodes<int, Item>(tree_, grid);
	
	auto draw = [this](auto const& item)
	{
		bwgui::SetBrushColor(renderer(), item.color);
		bwgui::FillCircle(renderer(), item.circle);
		bwgui::SetBrushColor(renderer(), bwgui::White);
		bwgui::DrawCircle(renderer(), item.circle);
	}; 
	taiga::traverse_items<int, Item>(tree_, draw);
}

int main()
{
	QuadTreeGui app;

	return app.Loop();
}
