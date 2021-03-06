#ifndef FIELD2048H
#define FIELD2048H

#include <array>
#include <random>
#include <ctime>
#include <utility>
#include <algorithm>

namespace field_2048 {

	// N - размер поля
	template<unsigned N> class Field2048 {
	public:
		Field2048();
		// функции сдвига, если после сдвига ходов не осталось, вернут false.
		bool move_up() { return move_ver(true); }
		bool move_down() { return move_ver(false); }
		bool move_left() { return move_hor(false); }
		bool move_right() { return move_hor(true); }
		std::array<std::array<unsigned, N>, N> get_tiles() const { return tiles; };	// получить плитки
		// начать игру заново. очистить поле и добавить 2 плитки
		void restart();
	private:
		void add_tile(unsigned _rank = 1);	// добавить плитку
		bool move_hor(bool _to_right);	// сдвиг по горизонтали, направление сдвига зависит от параметра
		bool move_ver(bool _to_up);	// сдвиг по вертикали, направление сдвига зависит от параметра
		void clockwise_rotation() { rotation(true); }	// поворот поля по часовой стрелке
		void counterclockwise_rotation() { rotation(false); }	// поворот поля против часовой стрелки
		void rotation(bool _is_clockwise); // поворот поля, направление поворота зависит от параметра
		bool check() const;	// проверка наличия ходов. если ходов нет, вернет false
		bool is_exist_empty() const;	// есть ли пустое место
		bool is_clash_avaiable() const; // есть ли рядом одинаковые плитки
		bool is_hor_clash_avaiable() const; // есть ли рядом по горизонтали одинаковые плитки
		bool is_ver_clash_avaiable() const; // есть ли рядом по вертикали одинаковые плитки
		// можно ли сдвинуть плитки по горизонтали, направление сдвига зависит от параметра
		bool is_hor_move_avaiable(bool _to_right) const;
		std::array<std::array<unsigned, N>, N> tiles;	// плитки
		std::default_random_engine rand_engine;	// процесор случайных чисел для этого объекта поля
		// распределение случайных чисел для этого объекта поля
		// используется для поиска свободного места для добавления новой плитки
		std::uniform_int_distribution<unsigned> unif_distr;
		// распределение Бернулли для этого объекта поля
		// используется при добавлении новых плиток. По умолчанию 90 %, что появится "2" и 10 %, что "4"
		std::bernoulli_distribution bern_distr;
	};

	// конструктор
	// задание начального значения процесора случайных чисел и диапазонов распределений
	template<unsigned N> inline Field2048<N>::Field2048() : rand_engine(static_cast<unsigned>(time(0))), unif_distr(0, N - 1), bern_distr(0.9) {
		restart();
	}

	template<unsigned N> inline void Field2048<N>::restart() {
		// сначала плиток нет
		for (auto &i : tiles) {
			for (auto &j : i) {
				j = 0;
			}
		}
		// добавить начальные плитки
		add_tile();
		add_tile();
	}

	template<unsigned N> inline void Field2048<N>::add_tile(unsigned _rank = 1) {
		std::pair<unsigned, unsigned> new_tile;
		// пока не найдется свободное место
		do {
			new_tile.first = unif_distr(rand_engine);
			new_tile.second = unif_distr(rand_engine);
		} while (tiles[new_tile.first][new_tile.second] != 0);
		// добавление плитки
		tiles[new_tile.first][new_tile.second] = _rank;
	}

	template<unsigned N> inline bool Field2048<N>::check() const {
		return is_exist_empty() || is_clash_avaiable();
	}

	template<unsigned N> bool Field2048<N>::move_hor(bool _to_right) {
		// если сдвиг возможен, он имеет место быть
		if (is_hor_move_avaiable(_to_right)) {
			for (unsigned i = 0; i != N; ++i) {
				// сдвиг
				stable_partition(tiles[i].begin(), tiles[i].end(),
					[_to_right](unsigned _u){ return (_u == 0) == _to_right; });
				// проверка на столкновение одинаковых плиток
				bool is_clashed = false;	// было ли хоть 1 столкновение в текущем обрабатываемом ряду
				for (unsigned j = 0; j != N - 1; ++j) {
					if (tiles[i][j] != 0 && tiles[i][j] == tiles[i][j + 1]) {
						++tiles[i][j];
						tiles[i][j + 1] = 0;
						is_clashed = true;
					}
				}
				// если столкновения были, то надо сдвинуть еще раз
				if (is_clashed) {
					stable_partition(tiles[i].begin(), tiles[i].end(),
						[_to_right](unsigned _u){ return (_u == 0) == _to_right; });
				}
			}
			// добавление новой плитки
			add_tile(bern_distr(rand_engine) ? 1 : 2);
			// если остались ходы, то вернем true;
			return check();
		}
		// в противном случае ничего не происходит
		else {
			return true;
		}
	}

	template<unsigned N> inline bool Field2048<N>::move_ver(bool _to_up) {
		bool check_result;	// результат проверки на наличие ходов
		clockwise_rotation();	// повернем поле по часовой стрелке
		check_result = move_hor(_to_up);	// выполним сдвиг по горизонтали
		counterclockwise_rotation();	// вернем нормальную ориентацию, повернув против часовой стрелки
		// вернем результат проверки на наличие ходов, полученный при сдвиге по горизонтали
		return check_result;
	}

	template<unsigned N> inline void Field2048<N>::rotation(bool _is_clockwise) {
		// temp будет повернутым tiles
		auto temp = tiles;
		for (unsigned j = 0; j != N; ++j) {
			for (unsigned i = 0; i != N; ++i) {
				temp[i][j] = (_is_clockwise ? tiles[N - j - 1][i] : tiles[j][N - i - 1]);
			}
		}
		// присваиваем повернутый массив полю класса
		tiles = temp;
	}

	template<unsigned N> inline bool Field2048<N>::is_exist_empty() const {
		// если хоть 1 ячейка пустая, вернет true
		for (const auto &i : tiles) {
			for (const auto &j : i) {
				if (j == 0) return true;
			}
		}
		// в противном случае - false
		return false;
	}

	template<unsigned N> inline bool Field2048<N>::is_clash_avaiable() const {
		return is_hor_clash_avaiable() || is_ver_clash_avaiable();
	}

	template<unsigned N> inline bool Field2048<N>::is_hor_clash_avaiable() const {
		for (unsigned i = 0; i != N; ++i) {
			for (unsigned j = 0; j != N - 1; ++j) {
				if (tiles[i][j] != 0 && tiles[i][j] == tiles[i][j + 1]) return true;
			}
		}
		return false;
	}

	template<unsigned N> inline bool Field2048<N>::is_ver_clash_avaiable() const {
		for (unsigned j = 0; j != N; ++j) {
			for (unsigned i = 0; i != N - 1; ++i) {
				if (tiles[i][j] != 0 && tiles[i][j] == tiles[i + 1][j]) return true;
			}
		}
		return false;
	}

	template<unsigned N> inline bool Field2048<N>::is_hor_move_avaiable(bool _to_right) const {
		// если возможен сдвиг без столкновений, вернет true
		for (unsigned i = 0; i != N; ++i) {
			if (!is_partitioned(tiles[i].begin(), tiles[i].end(),
				[_to_right](unsigned _u){ return (_u == 0) == _to_right; })) {
				return true;
			}
		}
		// если возможны столкновения одинаковых плиток, тоже вернет true
		if (is_hor_clash_avaiable()) return true;
		return false;
	}

}	// namespace field_2048 end

#endif
