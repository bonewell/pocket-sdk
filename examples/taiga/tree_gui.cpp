#include "bwgui/action.h"
#include "bwgui/application.h"

#include "taiga/tree.h"
#include "taiga/view.h"

template<typename T>
class TreeGui: public bwgui::Application
{
public:
	explicit TreeGui(taiga::Tree<T> tree);

	void OnLoop() override {}
	void OnRender() override;
	
private:
	taiga::TreeView<T, true> view_;
};

template<typename T>
TreeGui<T>::TreeGui(taiga::Tree<T> tree)
	: Application("tree_gui")
{
	view_ = taiga::CreateView<T, true>(tree);
}

template<typename T>
void TreeGui<T>::OnRender()
{
	taiga::DrawTree(*this, view_);
}

int main()
{
	//auto tree = taiga::make_tree<char>();
	//auto tree = taiga::make_tree<char>({'B'});
	//auto tree = taiga::make_tree<char>({'B', 'A'});
	//auto tree = taiga::make_tree<char>({'B', 'C'});
	//auto tree = taiga::make_tree<char>({'B', 'A', 'C'});
	//auto tree = taiga::make_tree<char>({'8', '7', '6'});
	//auto tree = taiga::make_tree<char>({'6', '7', '8'});
	//auto tree = taiga::make_tree<char>({'9', '7', '6', '8'});
	//auto tree = taiga::make_tree<char>({'6', '8', '7', '9'});
	//auto tree = taiga::make_tree<char>({'E', 'B', 'A', 'D', 'C', 'F', 'I', 'G', 'H', 'J'});
	//auto tree = taiga::make_tree<char>({'5', '3', '4', '7', '9', '2', '0', '8', '6', '1'});
	auto tree = taiga::make_tree<char>({'5', '2', '6', '1', '4', '9', '3', '7', '8'});

	TreeGui app{std::move(tree)};

	return app.Execute();
}
