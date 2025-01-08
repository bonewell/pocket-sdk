#include "bwgui/action.h"
#include "bwgui/application.h"

#include "taiga/thread_tree.h"
#include "taiga/view.h"

template<typename T>
class TreeGui: public bwgui::Application
{
public:
	explicit TreeGui(taiga::ThreadTree<T> tree);

	void OnLoop() override {}
	void OnRender() override;
	
private:
	taiga::ThreadTreeView<T> view_;
};

template<typename T>
TreeGui<T>::TreeGui(taiga::ThreadTree<T> tree)
	: Application("thread_tree_gui")
{
	view_ = taiga::CreateView(tree);
}

template<typename T>
void TreeGui<T>::OnRender()
{
	DrawThreadTree(*this, view_);
}

int main()
{
	//auto thread_tree = taiga::make_thread_tree<char>({'5', '3', '4', '7', '9', '2', '0', '8', '6', '1'});
	//auto thread_tree = taiga::make_thread_tree<char>({'5', '2', '6', '1', '4', '9', '3', '7', '8'});
	auto thread_tree = taiga::make_thread_tree<char>({'E', 'B', 'A', 'D', 'C', 'F', 'I', 'G', 'H', 'J'});

	TreeGui app{std::move(thread_tree)};

	return app.Loop();
}
