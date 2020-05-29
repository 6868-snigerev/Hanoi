// Honoi.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
// Программа разработана студентом МГТУ им. Н.Э.Баумана Большан Дмитрием

#include <Windows.h> 
#include <conio.h>
#include <iostream>

#define BLUE 1
#define RED 4
#define GREEN 10
#define WHITE 15
#define FORWARD 1
#define BACKWARD -1
#define ESC 27
#define ENTER 13
#define PC 0
#define MYSELF 1
#define ZERO 10
#define ONE 11
#define TWO 12

HANDLE hConsole;
int ww;
int wh;
int solve = -1;
using namespace std;

class stroka {
public:
    int number;
    int color;

    bool operator==(const stroka& give) {
        return (this->number == give.number) & (this->color == give.color);
    }

    void operator=(const stroka& given) {
        this->number = given.number;
        this->color = given.color;
    }

    friend std::ostream& operator<<(std::ostream& out, const stroka& given) {
        return out << given.number << ' ' << given.color;
    }
};

void print_bashnya(stroka** arr, int size, int max_row_size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < max_row_size; j++) {
            cout << arr[i][j].number << ", ";
        }
        cout << '\n';
        for (int j = 0; j < max_row_size; j++) {
            cout << arr[i][j].color << ", ";
        }
        cout << '\n' << '\n';
    }
}

// переводит курсор на позицию X, Y
void GoToXY(int column, int line) {
    COORD coord;
    coord.X = column;
    coord.Y = line;

    if (!SetConsoleCursorPosition(hConsole, coord)) {
        cout << "error: " << coord.X << coord.Y << endl;
    }
}

void print_colored(const char* text, int color) {
    SetConsoleTextAttribute(hConsole, color);
    cout << text;
}

void update_win_sizes() {
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    ww = consoleInfo.srWindow.Right - consoleInfo.srWindow.Left + 1;
    wh = consoleInfo.srWindow.Bottom - consoleInfo.srWindow.Top + 1;
}

void clear_all() {
    system("cls");
}

// рисует одну полоску, толщиной в (height-cl.numer)*2+1
void draw_cell(stroka cl, int height, int x, int y) {
    int thickness = height - cl.number + 1;
    if (x - thickness > 0) {
        int color;
        if (cl.number == 0) {
            GoToXY(x, y);
            print_colored("|", GREEN);
            // цвет последней ячейки - то, на что наводится в данный момент игрок (выбрал)
            if (cl.color == 1) {
                GoToXY(x, y - 2);
                print_colored("|", GREEN);
                GoToXY(x - 1, y - 1);
                print_colored("\ /", GREEN);
            }
        }
        else {
            GoToXY(x - thickness, y);
            if (cl.color == 1) {
                color = BLUE;
            }
            else {
                color = RED;
            }
            for (int i = 0; i < thickness * 2 + 1; i++) {
                print_colored("#", color);
            }
        }
    }
}

void update_frame(stroka** arr, int height, int max_row_size, int counter) {
    //int counter = 0;
    clear_all();
    update_win_sizes();
    GoToXY(0, 0);

    // пишем верхний текст
    if (solve == MYSELF) {
        print_colored("Для выбора башни нажмите -", GREEN);
        print_colored(" '1', '2', '3'", RED);
        cout << '\n';
        print_colored("Для отмены выбора или выхода нажмите -", GREEN);
        print_colored(" 'Esc'", RED);
    }
    else {
        print_colored("Для перемотки вперёд или выхода нажмите -", GREEN);
        print_colored(" 'Enter'", RED);
        cout << '\n';
        print_colored("Для отмены выбора или выхода нажмите -", GREEN);
        print_colored(" 'Esc'", RED);
    }

    /*
      ставим начальное положение башни по Y, начиная сверху; это положение - (оставшееся место от нижнего текста "-" (собственная высота "+"
      высота стрелки равная 2м)),
      или 6 (иначе выйдет в минус)
    */
    int tower_y = 4;

    // чертит чёрточки по строчно; если столбец совпадает с 1/4, 2/4, 3/4 экрана, то чертит чёрточку
    for (int y = 0; y < max_row_size; y++) {
        GoToXY(0, y);
        for (int x = 1; x < ww; x++) {
            if ((x % (ww / 4) == 0) && (x / (ww / 4) <= 3)) {
                //GoToXY(x-1,tower_y+y);
                //cout << "|";
                //cout << "x/(ww/4) " << x/(ww/4) << " y: " << y;
                draw_cell(arr[x / (ww / 4) - 1][y], height, x - 1, tower_y + height - 1 - y);
            }
        }
    }
    //int bottom_y = max(wh-2,);
    GoToXY(0, tower_y + height + 1);
    print_colored("Количество ходов: ", GREEN);
    cout << counter;
}

void show_victory() {
    const char* victory = "ПОБЕДА";
    int ln = 6;
    int th = 1;
    for (int y = 0; y < 3; y++) {
        GoToXY(ww / 2 - ln / 2 - th, wh / 2 - th + y);
        for (int x = 0; x < ln + th * 2; x++) {
            print_colored("#", GREEN);
        }
    }
    GoToXY(ww / 2 - ln / 2, wh / 2);
    print_colored(victory, WHITE);
}



/*
  копирует массив arr1 в arr2;
  ввод:
    - arr1 - входной массив
    - arr2 - выходной массив
    - height - высота башен (максимальная)
*/
void copy_array(stroka** arr1, stroka** arr2, int height) {
    for (int x = 0; x < 3; x++) {
        for (int y = 0; y < height; y++) {
            arr2[x][y] = arr1[x][y];
        }
    }
}

// возвращает -1, если прожал Esc и +1, если Enter; удобно для навигации
int navigating() {
    int where_to_go = -10;
    while (where_to_go == -10) {
        char key = _getch();
        if (solve == PC) {
            if (key == ESC) {
                where_to_go = BACKWARD;
            }
            else if (key == ENTER) {
                where_to_go = FORWARD;
            }
        }
        else {
            if (key == ESC) {
                where_to_go = BACKWARD;
            }
            else if (key == '1') {
                where_to_go = 0;
            }
            else if (key == '2') {
                where_to_go = 1;
            }
            else if (key == '3') {
                where_to_go = 2;
            }
        }
    }
    return where_to_go;
}

class history_list {
public:
    unsigned int counter;
    unsigned int done_steps_counter;
    unsigned int height;
    unsigned int max_row_size;
    stroka*** moves;
    stroka* moved_cells;
    unsigned int len;
    void create_history(stroka** arr, int height, int max_row_size) {
        len = 100000;
        this->max_row_size = max_row_size;
        this->height = height;
        moves = new stroka * *[len];
        for (int i = 0; i < len; i++) {
            moves[i] = new stroka * [3];
            for (int j = 0; j < 3; j++) {
                moves[i][j] = new stroka[max_row_size];
            }
        }
        moved_cells = new stroka[len];
        counter = 0;
        set_start_array(arr);
    }
    /*
      - В список ходов записывает стартовый массив, не влияя на счётчик ходов
    */
void set_start_array(stroka** arr) {
        copy_array(arr, moves[0], max_row_size);
}
    /*
      - Добавляет новый массив arr (состояние башен) к собственному массиву move (история состояний (положений) башен)
      - Увиличивает счётчик движений
    */
void append(stroka** arr, stroka who_moves) {
        counter++;
        done_steps_counter++;
        copy_array(arr, moves[counter], max_row_size);
        moved_cells[counter] = who_moves;
}
    /*
      Добавить тут запись 3!!!!!!
      - Добавляет новый ход в историю, но перед этим:
      -- Просто добавляет новый ход в историю, ничего перед этим не делая, если цепочка ходов не была бесполезна
      -- Удаляет прерыдущипй ход, если он был бесполезен
      Принцип работы:
      пред. ход:
       2-1  |   |
       2-0  |   |
       1-0 1-1  |   2-1 перекинули со 2й башни на 1ю
      текущий ход:
        |   |   |
       2-0  |   |
       1-0 1-1 2-1  2-1 перекинули с 1й башни на 2ю
      2 раза подряд походили 1й ячекой - бесполезно
      - переводим счётчик назад -> теперь счётчик стоит на предыдущем ходе, при использовании функции append он перезапишется
      - вносим новый ход в историю
    */
    void append_no_useless(stroka** arr, stroka who_moves) {
        if (counter >= 1) {
            if (who_moves == moved_cells[counter])
            {
                counter--;
            }
        }
        append(arr, who_moves);
    }
    void print() {
        for (int i = 0; i <= counter; i++)
        {
            cout << "----------" << '\n';
            print_bashnya(moves[i], 3, height);
            cout << "-" << '\n';
            cout << moved_cells[i] << '\n';
            cout << "----------" << '\n';
        }
    }
    void draw() {
        int i = 0;
        while (i <= counter)
        {
            update_frame(moves[i], height, max_row_size, i);
            if (i == counter) {
                show_victory();
            }
            i += navigating();
        }
    }
};

history_list history;

// элмент башни from, находящемся на высоте from_i, меняется местами с элементво башни to, находящемся на высоте to_i
void swap_cell_elem(stroka** arr, int from, int from_i, int to, int to_i) {
    stroka per = arr[from][from_i];
    arr[from][from_i] = arr[to][to_i];
    arr[to][to_i] = per;
}

/*
  ищем последние элементы башен from и вершину башни to и перекладывает верхний элемент с from на to
  пример:
  - arr
    |   |   |
   2-0 2-1  |
   1-0 1-1  |
  - from = 1
  - to = 0
  - arr после работы процедуры
   2-1  |   |
   2-0  |   |
   1-0 1-1  |
*/
void move_cell(stroka** arr, int from, int to) {
    int from_i = 0;
    int i = 0;
    while (arr[from][i].number != 0) {
        from_i = i;
        i += 1;
    }
    int to_i = 0;
    i = 0;
    while (arr[to][i].number != 0){
        i += 1;
        to_i = i;
    }

    if (solve == PC) {
        swap_cell_elem(arr, from, from_i, to, to_i);
        history.append_no_useless(arr, arr[to][to_i]);
    }
    else {
        // ! выполнение условий
        // если размер ячейки <= размеру места, куда она идёт (! размер = высота-номер+1), или место пусто, то передвинуть
        if (to_i != 0) {
            if ((arr[from][from_i].number >= arr[to][to_i - 1].number) || (arr[to][to_i - 1].number == 0))  {
                swap_cell_elem(arr, from, from_i, to, to_i);
                history.append(arr, arr[to][to_i]);
            }
        }
        else {
            swap_cell_elem(arr, from, from_i, to, to_i);
            history.append(arr, arr[to][to_i]);
        }
    }
}

// делает тоже самое то и move + меняет цвет передвигаемой ячейки на противоположный
void move_magnetic_cell(stroka** arr, int from, int to) {
    int from_i = 0;
    int i = 0;
    while (arr[from][i].number != 0) {
        from_i = i;
        i += 1;
    }
    int to_i = 0;
    i = 0;
    while (arr[to][i].number != 0) {
        i += 1;
        to_i = i;
    }

    if (solve == PC) {
        swap_cell_elem(arr, from, from_i, to, to_i);
        arr[to][to_i].color = (arr[to][to_i].color != 1);
        history.append_no_useless(arr, arr[to][to_i]);
    }
    else {
        // ! выполнение условий
        // если размер ячейки <= размеру места, куда она идёт (! размер = высота-номер+1), или место пусто, то передвинуть
        // и полярности ячейки и места не совпадают
        print_bashnya(arr, 3, 4);
        if (to_i != 0) {
            if (((arr[from][from_i].number >= arr[to][to_i - 1].number) || (arr[to][to_i - 1].number == 0))) {
                if (arr[from][from_i].color != arr[to][to_i - 1].color) {
                    swap_cell_elem(arr, from, from_i, to, to_i);
                    arr[to][to_i].color = (arr[to][to_i].color != 1);
                    history.append(arr, arr[to][to_i]);
                }
            }
        }
        else {
            swap_cell_elem(arr, from, from_i, to, to_i);
            arr[to][to_i].color = (arr[to][to_i].color != 1);
            history.append(arr, arr[to][to_i]);
        }
    }
}


// получает длинну массива arr (длинна - кол-во не нулевых элементов, идущих подряд с нуля)
int get_len(stroka* arr) {
    int elem = 0;
    int i = 0;
    while (arr[i].number != 0) {
        i += 1;
    }
    return i;
}



/*
  Классические башни
*/
// решение ханойских башен через рекурсию
void solve_hanoi(stroka** arr, int height, int s, int t, int u, int n) {
    if (n == 1) {
        move_cell(arr, s, t);
    }
    else {
        solve_hanoi(arr, height, s, u, t, n - 1);
        move_cell(arr, s, t);
        solve_hanoi(arr, height, u, t, s, n - 1);
    }
}

void solve_simple_hanoi(stroka** arr, int height, int s, int t, int u) {
    solve_hanoi(arr, height, s, t, u, height);
}



/*
  Цветные башни
*/

/*
  решает подзадачу двухцветной ханойской башни по перемещению сдвоенной башни
    - пример визуализации входного массива:
        3-1  |   |
        3-0  |   |
        2-1  |   |
        2-0  |   |
        1-1  |   |
        1-0  |   |
        Где 0 - один цвет, 1 - другой
  на вход:
    * массив с башнями
    * размер башни / 2 (кол-во сдвоенных элементов)
    * номер стартовой башни (с нуля)
    * номер целевой башни
    * первый раз - размер башни / 2 (кол-во сдвоенных элементов)
  - пример визуализации входного массива с s=0, t=2:
         |   |  3-1
         |   |  3-0
         |   |  2-1
         |   |  2-0
         |   |  1-0
         |   |  1-1
*/
void DoubleTowersOfHanoi(stroka** arr, int height, int s, int t, int u, int n) {
    if (n == 1) {
        move_cell(arr, s, t);
        move_cell(arr, s, t);
    }
    else if (n != 0) {
        DoubleTowersOfHanoi(arr, height, s, u, t, n - 1);
        move_cell(arr, s, t);
        move_cell(arr, s, t);
        DoubleTowersOfHanoi(arr, height, u, t, s, n - 1);
    }
}




/*
  решает подзадачу двухцветной ханойской башни по переносу башни без переворачивания нижнего элемента
    - пример визуализации входного массива:
        3-1  |  |
        3-0  |  |
        2-1  |  |
        2-0  |  |
        1-1  |  |
        1-0  |  |
        Где 0 - один цвет, 1 - другой
  на вход:
    * массив с башнями
    * кол-во башен
    * размер башни
    * номер стартовой башни (с нуля)
    * номер целевой башни
  принцип работы:
    т.к. это башня по строению схожа с простой ханойской башней - дублирует каждый элемент, то будет достаточно дважды за цикл перемещать ячейки
  пример входного и выходного массива с s=0, t=1, u=2:
    3-1  |  |      |  3-1  |
    3-0  |  |      |  3-0  |
    2-1  |  |      |  2-1  |
    2-0  |  |      |  2-0  |
    1-1  |  |      |  1-1  |
    1-0  |  |      |  1-0  |
    использует предыдущую подзадачу, но в начале перекладывает не на целевой стержень, там нижняя ячейка переворачивается, а после на целевой, где
    повторно переворачивается и возвращается в норму
    (1 шаг - S-стартовый стержень, U-целевой стержень, T-оставшийся
     2 шаг - U-стартовый стержень, T-целевой стержень, S-оставшийся)
*/
void EnhancedDoubleTowersOfHanoi(stroka** arr, int height, int s, int t, int u) {
    DoubleTowersOfHanoi(arr, height / 2, s, u, t, height / 2);
    if (height % 2 == 1) {
        move_cell(arr, s, t);
    }
    DoubleTowersOfHanoi(arr, height / 2, u, t, s, height / 2);
}




/*
  решает подзадачу двухцветной ханойской башни по сбору 2 башен в 1
    - пример визуализации входного массива:
        3-0  3-1  |
        2-0  2-1  |
        1-0  1-1  |
        Где 0 - один цвет, 1 - другой
  на вход:
    * массив с башнями
    * размер маленькой башни (размер под_башни / кол-во сдвоенных элементов)
    * номер первой башни (с нуля)
    * номер второй башни
    * оставшийся столб
  принцип работы:
    - вначале перекидывает верхнюю ячейку второй башни на первую (с T на S)
        - с первого столба получившуюся разноцветную башню из двух (4х,6х и ТД) элементов перекидывает на свободную
        - перекидывает верхнюю ячейку второй башни на первую (с T на S), что бы получилась ещё башня из 2х (4х,6х и ТД) элментов
        - с оставшегося столба перекладывает старую башню из 2 элементов на первую, получается башня из 4х (6х,8х и ТД) элементов
    - повторяет цикл столько раз, сколько элементов в одной под_башне получается, что мы сначала накладываем маленькую цветную башню
    -> освобождаем место
    -> докладываем верхний элемент со второй на первую, что-бы добавить к ней 2 элемента
    -> а потом докидываем остаток на получившуюся башенку
    -> и по циклу
*/
void MergeProblem(stroka** arr, int sub_height, int s, int t, int u) {
    move_cell(arr, t, s);
    for (int step = 1; step < sub_height; step++) {
        DoubleTowersOfHanoi(arr, step, s, u, t, step);
        move_cell(arr, t, s);
        DoubleTowersOfHanoi(arr, step, u, s, t, step);
    }
}

void Disassamble(stroka** arr, int height, int s, int t, int u) {
    EnhancedDoubleTowersOfHanoi(arr, height, s, t, u); //s->t
    for (int step = 0; step < height / 2; step++) {
        if ((step % 2) == 0) {
            EnhancedDoubleTowersOfHanoi(arr, height - step * 2 - 1, t, s, u); //t->s
        }
        else {
            EnhancedDoubleTowersOfHanoi(arr, height - step * 2 - 1, s, t, u); //s->t
        }
    }
}

void solve_bicolor_hanoi(stroka** arr, int height, int s, int t, int u) {
    MergeProblem(arr, height, s, t, u);
    Disassamble(arr, height * 2, s, t, u);
}





/*
  Магнитные ханои
*/

/*
  Заглавные буквы - полярность того, что находится снизу передвигаемых ячеек (кругов)
  Соответственно BRB - это S - отрицательный, U - положительный, T - отрицательный
*/
void BRB(stroka** arr, int s, int t, int u, int n) {
    if (n == 1) {
        move_magnetic_cell(arr, s, u);
        move_magnetic_cell(arr, u, t);
    }
    /*
      если на стартовой позиции высота равна 2м, то это значит, что когда 1й (меньший по размеру кольца) элемент уйдёт на целевую башню, а 2й на запосную башню,
      то стартовая позиция станет равна N (пуста) и на неё можно будет свободно положить R (положительный) элемент, который по возвращению на с S на T станет
      снова отрицательным
      (был -, положили на S стал +, обратно стал -)
    */
    else if ((n == 2) && (get_len(arr[s]) == 2)) {
        BRB(arr, s, t, u, n - 1);
        move_magnetic_cell(arr, s, u);
        move_magnetic_cell(arr, t, s); // BRN
        move_magnetic_cell(arr, u, t);
        move_magnetic_cell(arr, s, t); // NRB (RRB)
    }
    else {
        BRB(arr, s, t, u, n - 1);             // освобождает место n-1 с S на T
        move_magnetic_cell(arr, s, u);
        BRB(arr, t, s, u, n - 1);             // освобождает место n-1 с T на S
        move_magnetic_cell(arr, u, t);
        BRB(arr, s, t, u, n - 1);             // перемещает остаток n-1 с S на T
    }
}

void RNN(stroka** arr, int s, int t, int u, int n) {
    if (n == 1)
    {
        move_magnetic_cell(arr, s, t);
    }
    else {
        RNN(arr, s, u, t, n - 1);
        move_magnetic_cell(arr, s, t);
        BRB(arr, u, t, s, n - 1);
    }
}

void solve_magnetic_hanoi(stroka** arr, int height, int s, int t, int u) {
    RNN(arr, s, t, u, height);
}




/*
  Инициализация классической ханои
  - стартовую башню s заполняет кругами толщиной от 1 до height, цвета 0
  - в конце каждой башни всегда должна быть пустая (нулевая) ячейка, нужно для работы функций, что бы не передавать высоту каждый раз через аргументы
*/
void init_classic(stroka** arr, int height, int s, int t) {
    for (int i = 0; i < height; i++) {
        arr[s][i].number = i + 1; // +1
        arr[s][i].color = 0;
    }
}


/*
  Инициализация двухцветной ханои
  - заполняет башню s чередующимеся цветами (0 - цвет нижнего круга/ячейки)
  - заполняет башню t чередующимеся цветами (1 - цвет нижнего круга/ячейки)
*/
void init_bicolor(stroka** arr, int height, int s, int t) {
    for (int i = 0; i < height; i++) {
        arr[s][i].number = i + 1;
        arr[s][i].color = 1 - i % 2;
    }

    for (int i = 0; i < height; i++) {
        arr[t][i].number = i + 1;
        arr[t][i].color = i % 2;
    }
}

void init_magnetic(stroka** arr, int height, int s, int t) {
    init_classic(arr, height, s, t);
}


bool check_win(stroka** arr, int height, int s, int t, int u, char type) {
    bool fl = false;
    if (type == 'c') {
        if (get_len(arr[t]) == height) {
            fl = true;
        }
    }
    else if (type == 'b') {
        if ((get_len(arr[s]) == height) && (get_len(arr[t]) == height)) {
            fl = true;
            //cout << height;
            for (int i = 0; i < height; i++)  {
                fl = fl && (arr[s][i].color == 0);  // все ли кружки из первого столба цвета 0 (противоположного начальному)
                fl = fl && (arr[t][i].color == 1);  // аналогично второй столб
                //cout << endl << (arr[t][i].color==1);
            }
        }
    }
    else {
        if (get_len(arr[t]) == height) {
            fl = true;
            for (int i = 0; i < height; i++) {
                fl = fl && (arr[t][i].color == 1);  // все ли кружки из целевого столба повеёрнуты минусом вверх (условие задачи)
            }
        }
    }
    return fl;
}



int main()
{
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    // для того, что бы печатать русским
    SetConsoleOutputCP(1251);
    int height = 1;
    int s;
    int t;
    int u;
    char type = 'n';
    stroka** cols;
    int in;
    GoToXY(0, 0);
    print_colored("ПРАВИЛА ИГРЫ:\n", RED);
    print_colored("Для перемещения колец выберите номер башни с которой хотите перенести, затем ту на которую переносить. Нажимать Enter при игре НЕ НУЖНО!\n", BLUE);
    print_colored("Выберете тип\n", RED);
    print_colored("'1' - классическая (обычная игра ханойскими башнями)\n'2' - двухцветная (классическая игра, только с участием двух цветов (-> две башни)\n'3' - магнитная (почти то же самое, что и классическая, только два цвета и при ходе переворачиваются диски  (цвета дисков должны чередоваться)\n", GREEN);
    while (type == 'n') {
        cin >> in;
        if (in == 1) {
            type = 'c';
        }
        else if (in == 2) {
            type = 'b';
        }
        else if (in == 3) {
            type = 'm';
        }
    }
    clear_all();
    GoToXY(0, 0);
    print_colored("Введите высоту башни (количество блинов в ней): \n", GREEN);
    cin >> height;
    clear_all();
    GoToXY(0, 0);
    print_colored("Выберете тип решения\n", RED);
    print_colored("'1' - я сам\n'2' - лень матушка, пусть сделает компьютер (жми просто Enter если выберешь)\n", GREEN);
    while (solve == -1) {
        cin >> in;
        if (in == 1)  {
            solve = MYSELF;
        }
        else if (in == 2) {
            solve = PC;
        }
    }

    int max_row_size;
    if (type == 'b') {
        max_row_size = height * 2 + 1;
        s = 0;
        t = 1;
        u = 2;
    }
    else {
        max_row_size = height + 1;
        s = 0;
        u = 1;
        t = 2;
    }

    cols = new stroka * [3];
    // заполняет пространство 3 на height (кол-во башен на высоту+1), что-бы потом нормально инициализировать можно было
    for (int i = 0; i < 3; i++) {
        cols[i] = new stroka[max_row_size];
        for (int j = 0; j < max_row_size; j++) {
            cols[i][j].number = 0;
            cols[i][j].color = 0;
        }
    }

    if (type == 'c') {
        init_classic(cols, height, s, t);
        history.create_history(cols, height, max_row_size);
    }
    else if (type == 'b') {
        init_bicolor(cols, height, s, t);
        history.create_history(cols, height, max_row_size);
    }
    else {
        init_magnetic(cols, height, s, t);
        history.create_history(cols, height, max_row_size);
    }

    if (solve == PC) {
        if (type == 'c')  {
            solve_simple_hanoi(cols, height, s, t, u);
        }
        else if (type == 'b') {
            solve_bicolor_hanoi(cols, height, s, t, u);
        }
        else {
            solve_magnetic_hanoi(cols, height, s, t, u);
        }
        history.draw();
    }
    else {
        int from_to[2];
        // пока не выиграл
        while (!check_win(cols, height, s, t, u, type)) {
            // обнуляет откуда ходить и стрелки, идет постоянное обновление картинки
            from_to[0] = 0;
            from_to[1] = 0;
            update_frame(cols, height, max_row_size, history.done_steps_counter);
            // пока хотя бы 1 клавиша - ESC, или клавишы равны друг другу,
            while ((from_to[0] == -1) || (from_to[1] == -1) || (from_to[0] == from_to[1]))  {
                if (history.counter < 0) {
                    cout << "NOO";
                    exit(1);
                }
                for (int i = 0; i < 2; i++) {
                    from_to[i] = navigating();
                    //cols[from_to[i]][max_row_size].color = 1;
                    if (from_to[i] == -1)  {
                        if (history.counter <= 0) {
                            exit(1);
                        }
                        from_to[0] = 0;
                        from_to[1] = 0;
                        history.counter -= 1;
                        cols = history.moves[history.counter];
                        update_frame(cols, height, max_row_size, history.done_steps_counter);
                        break;
                    }
                }
            }
            if (type == 'm') {
                move_magnetic_cell(cols, from_to[0], from_to[1]);
            }
            else {
                move_cell(cols, from_to[0], from_to[1]);
            }
        }
        update_frame(cols, height, max_row_size, history.done_steps_counter);
        show_victory();
        cin.get();
    }
    cin.get();
    cout << endl;
    cout << endl;
}
