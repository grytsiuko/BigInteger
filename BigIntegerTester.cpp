//*******************************************//
//       Developed by Oleksandr Hrytsiuk     //
//                  Project                  //
//*******************************************//


#include <iostream>
#include <cassert>
#include "BigInteger.h"
#include "BigIntegerTester.h"

using std::cout;
using std::endl;
using std::to_string;

void BigIntegerTester::test_all() {

    cout << "\nTesting BigInteger\n";

    test_converting();
    test_negating();
    test_shifts();
    test_add_sub();
    test_mul_div();
    test_compare();
    test_inc_dec();
    test_self_operator();
    test_power();
}

void BigIntegerTester::assert_expression
        (const string &a, binary_operator op, const string &b, const string &ans) {

    cout << a << " ";

    if (op == operator+) cout << "+";
    if (op == operator*) cout << "*";
    if (op == operator/) cout << "/";
    if (op == static_cast<binary_operator>(operator-)) cout << "-";

    cout << " " << b << " = " << ans << endl;
    assert(op(BigInteger(a), BigInteger(b)).to_string() == ans);
}

void BigIntegerTester::assert_expression
        (const string &a, shift_operator op, unsigned int shift, const string &ans) {

    cout << a << " ";

    if (op == operator>>) cout << ">>";
    if (op == static_cast<shift_operator>(operator<<)) cout << "<<";

    cout << " " << shift << " = " << ans << endl;
    assert(op(BigInteger(a), shift).to_string() == ans);
}

void BigIntegerTester::assert_expression
        (const string &a, cmp_operator op, const string &b) {

    cout << a << " ";

    if (op == operator==) cout << "==";
    if (op == operator!=) cout << "!=";
    if (op == operator<=) cout << "<=";
    if (op == operator>=) cout << ">=";
    if (op == operator<) cout << "<";
    if (op == operator>) cout << ">";

    cout << " " << b << endl;
    assert(op(BigInteger(a), BigInteger(b)));
}

void BigIntegerTester::assert_equal_expression
        (const BigInteger &a, const string &ans) {

    cout << a << endl
         << a.to_bits_string() << endl << endl;
    assert(a.to_string() == ans);
}

void BigIntegerTester::assert_power_expression
        (const string &a, unsigned int pow, const string &ans) {

    cout << a << " ^ " << pow << " = " << ans << endl << endl;
    assert(power(a, pow).to_string() == ans);
}

void BigIntegerTester::test_converting() {

    cout << "\nTesting converting\n\n";

    string number = "123456789012345678901234567890";
    assert_equal_expression(number, number);

    string neg_number = "-123456789012345678901234567890";
    assert_equal_expression(neg_number, neg_number);

    string zero = "0";
    assert_equal_expression(zero, zero);

    BigInteger a("12345");
    a = a;
    assert_equal_expression(a, "12345");

    BigInteger b("0");
    b = a;
    assert_equal_expression(b, "12345");

    try {
        BigInteger empty("");
        assert(false);
    } catch (const BigInteger::BigIntegerException &) {}

    try {
        BigInteger illegal("3j");
        assert(false);
    } catch (const BigInteger::BigIntegerException &) {}

    assert_equal_expression(-0, "0");
    assert_equal_expression(0, "0");
    assert_equal_expression(100, "100");
    assert_equal_expression(-100, "-100");
    assert_equal_expression(1234567, "1234567");
    assert_equal_expression(-1234567, "-1234567");

    cout << "\nSUCCESS!\n";
}

void BigIntegerTester::test_negating() {

    cout << "\nTesting negating\n\n";

    assert_equal_expression(-BigInteger::ZERO, "0");
    assert_equal_expression(-BigInteger::ONE + BigInteger::ONE, "0");

    assert_equal_expression(-BigInteger("255"), "-255");
    assert_equal_expression(-BigInteger("-1"), "1");
    assert_equal_expression(-BigInteger("-0"), "0");

    assert_equal_expression(-BigInteger::ZERO >> 4, "0");
    assert_equal_expression(-BigInteger::ZERO << 4, "0");
    assert_equal_expression(-BigInteger::ZERO + BigInteger("40000"), "40000");

    assert_equal_expression(-BigInteger("12345678901234567890"), "-12345678901234567890");
    assert_equal_expression(-BigInteger("-12345678901234567890"), "12345678901234567890");

    assert_equal_expression(-(-BigInteger("-12345678901234567890")), "-12345678901234567890");

    cout << "\nSUCCESS!\n";
}

void BigIntegerTester::test_shifts() {

    cout << "\nTesting shifts\n\n";

    assert_expression("-0", operator>>, 200, "0");
    assert_expression("0", operator<<, 200, "0");
    assert_expression("1", operator<<, 8, "256");
    assert_expression("-1", operator<<, 8, "-256");
    assert_expression("256", operator>>, 8, "1");
    assert_expression("-128", operator<<, 9, "-65536");

    assert_expression("88888888888888888889", operator>>, 2, "22222222222222222222");
    assert_expression("102410241024102410241025", operator>>, 8, "400040004000400040004");
    assert_expression("4194304419430441943044194305", operator>>, 12, "1024000102400010240001024");

    assert_expression("-88888888888888888889", operator>>, 2, "-22222222222222222223");
    assert_expression("-102410241024102410241025", operator>>, 8, "-400040004000400040005");
    assert_expression("-4194304419430441943044194305", operator>>, 12, "-1024000102400010240001025");

    assert_expression("22222222222222222222", operator<<, 2, "88888888888888888888");
    assert_expression("400040004000400040004", operator<<, 8, "102410241024102410241024");
    assert_expression("1024000102400010240001024", operator<<, 12, "4194304419430441943044194304");

    assert_expression("-22222222222222222222", operator<<, 2, "-88888888888888888888");
    assert_expression("-400040004000400040004", operator<<, 8, "-102410241024102410241024");
    assert_expression("-1024000102400010240001024", operator<<, 12, "-4194304419430441943044194304");

    BigInteger a("12345678901234567890");
    (a <<= 2) >>= 2;
    (a <<= 8) >>= 8;
    (a <<= 28) >>= 28;
    (a <<= 100) >>= 100;
    assert(a.to_string() == "12345678901234567890");

    cout << "\nSUCCESS!\n";
}

void BigIntegerTester::test_add_sub() {

    cout << "\nTesting addition and subtraction\n\n";

    assert_expression("100", operator+, "50", "150");
    assert_expression("128", operator+, "128", "256");
    assert_expression("111111111111111", operator+, "555555555555555", "666666666666666");

    assert_expression("-192", operator+, "-192", "-384");
    assert_expression("-64", operator+, "-64", "-128");
    assert_expression("-128", operator+, "-128", "-256");

    assert_expression("-111111111111111", operator+, "-555555555555555", "-666666666666666");
    assert_expression("-111111111111111", operator+, "555555555555555", "444444444444444");
    assert_expression("555555555555555", operator+, "-111111111111111", "444444444444444");
    assert_expression("111111111111111", operator+, "-555555555555555", "-444444444444444");
    assert_expression("-555555555555555", operator+, "111111111111111", "-444444444444444");

    assert_expression("100", operator-, "-50", "150");
    assert_expression("128", operator-, "-128", "256");
    assert_expression("111111111111111", operator-, "-555555555555555", "666666666666666");

    assert_expression("-192", operator-, "192", "-384");
    assert_expression("-64", operator-, "64", "-128");
    assert_expression("-128", operator-, "128", "-256");

    assert_expression("-111111111111111", operator-, "555555555555555", "-666666666666666");
    assert_expression("-111111111111111", operator-, "-555555555555555", "444444444444444");
    assert_expression("555555555555555", operator-, "111111111111111", "444444444444444");
    assert_expression("111111111111111", operator-, "555555555555555", "-444444444444444");
    assert_expression("-555555555555555", operator-, "-111111111111111", "-444444444444444");

    BigInteger a("12345");
    (a += BigInteger("1234567890")) -= BigInteger("1234567890");
    (a += BigInteger("12345678901234567890")) -= BigInteger("12345678901234567890");
    (a -= BigInteger("1234567890")) += BigInteger("1234567890");
    (a -= BigInteger("12345678901234567890")) += BigInteger("12345678901234567890");
    assert(a.to_string() == "12345");

    cout << "\nSUCCESS!\n";
}

void BigIntegerTester::test_mul_div() {

    cout << "\nTesting multiplication and division\n\n";

    assert_expression("1234567890", operator*, "0", "0");
    assert_expression("-1234567890", operator*, "0", "0");

    assert_expression("1234567890", operator*, "987654321", "1219326311126352690");
    assert_expression("-1234567890", operator*, "987654321", "-1219326311126352690");
    assert_expression("1234567890", operator*, "-987654321", "-1219326311126352690");
    assert_expression("-1234567890", operator*, "-987654321", "1219326311126352690");

    assert_expression("0", operator/, "1234567890", "0");
    assert_expression("0", operator/, "-1234567890", "0");

    assert_expression("1219326311126352690", operator/, "987654321", "1234567890");
    assert_expression("-1219326311126352690", operator/, "987654321", "-1234567890");
    assert_expression("1219326311126352690", operator/, "-987654321", "-1234567890");
    assert_expression("-1219326311126352690", operator/, "-987654321", "1234567890");

    assert_expression("101", operator/, "10", "10");
    assert_expression("-101", operator/, "10", "-11");

    try {
        BigInteger("12345") / BigInteger::ZERO;
        assert(false);
    } catch (const BigInteger::BigIntegerException &e) {
        cout << "Caught " << e.get_error_message() << endl;
    }

    BigInteger a("12345");
    (a *= BigInteger("9999999999999")) /= BigInteger("9999999999999");
    (a /= BigInteger("15")) *= BigInteger("15");
    (a *= BigInteger("-9999999999999")) /= BigInteger("-9999999999999");
    (a /= BigInteger("-15")) *= BigInteger("-15");
    assert(a.to_string() == "12345");

    cout << "\nSUCCESS!\n";
}

void BigIntegerTester::test_compare() {

    cout << "\nTesting compare\n\n";

    assert_expression("0", operator==, "0");
    assert_expression("0", operator<=, "0");
    assert_expression("0", operator>=, "0");

    assert_expression("1234567890", operator==, "1234567890");
    assert_expression("1234567890", operator<=, "1234567890");
    assert_expression("1234567890", operator>=, "1234567890");

    assert_expression("-1234567890", operator==, "-1234567890");
    assert_expression("-1234567890", operator<=, "-1234567890");
    assert_expression("-1234567890", operator>=, "-1234567890");

    assert_expression("54321", operator!=, "1234567890");
    assert_expression("54321", operator<, "1234567890");
    assert_expression("1234567890", operator!=, "54321");
    assert_expression("1234567890", operator>, "54321");

    assert_expression("-54321", operator!=, "-1234567890");
    assert_expression("-54321", operator>, "-1234567890");
    assert_expression("-1234567890", operator!=, "-54321");
    assert_expression("-1234567890", operator<, "-54321");

    assert_expression("54321", operator!=, "-1234567890");
    assert_expression("54321", operator>, "-1234567890");
    assert_expression("-1234567890", operator!=, "54321");
    assert_expression("-1234567890", operator<, "54321");

    assert_expression("-54321", operator!=, "1234567890");
    assert_expression("-54321", operator<, "1234567890");
    assert_expression("1234567890", operator!=, "-54321");
    assert_expression("1234567890", operator>, "-54321");

    cout << "\nSUCCESS!\n";
}

void BigIntegerTester::test_inc_dec() {

    cout << "\nTesting increments, decrements - ";

    int i = -5000;
    BigInteger a(i);
    BigInteger a_copy;

    for (; i <= 5000; i++, ++a) {
        a_copy = a++;
        assert(i == a_copy);
        a_copy = --a;
        assert(i == a_copy);
    }
    for (; i >= -5000; i--, --a) {
        a_copy = a--;
        assert(i == a_copy);
        a_copy = ++a;
        assert(i == a_copy);
    }


    cout << "SUCCESS!\n";
}

void BigIntegerTester::test_self_operator() {

    cout << "\nTesting a += a, a -= a, a *= a, a /= a  - ";

    BigInteger a("1000");
    a += a;
    assert(a == BigInteger("2000"));

    a = BigInteger("1000");
    a *= a;
    assert(a == BigInteger("1000000"));

    a = BigInteger("1000");
    a -= a;
    assert(a == BigInteger::ZERO);

    a = BigInteger("1000");
    a /= a;
    assert(a == BigInteger::ONE);

    cout << "SUCCESS!\n";
}

void BigIntegerTester::test_power() {

    cout << "\nTesting power\n\n";

    assert_power_expression("2", 100, (BigInteger::ONE << 100).to_string());
    assert_power_expression("2", 1000, (BigInteger::ONE << 1000).to_string());

    assert_power_expression("1234567", 0, BigInteger::ONE.to_string());
    assert_power_expression("-123456789", 10, "82252625914710257950476114366153554776"
                                              "4137892295514168093701699676416207799736601");
    assert_power_expression("-123456789", 15, "-2358982165591483812094703636914720394"
                                              "83181699385194041759684258234180082491158099126"
                                              "16071588527110255905622789563711716349");
    assert_power_expression("99999999999999999999", 99,
                            "9999999999999999990100000000000000004850999999999999998431510000000000000376"
                            "4375999999999999284768560000000000112052925599999999985112968456000000001712"
                            "0086275599999998268969054356000000155792785107959999987394947386719600000924"
                            "3705249738959999381382802517469600038000770702498295997846622993525096560113"
                            "0522928399324305944803880554621225062514489885845033080492803325919238063410"
                            "2878669632304774635986945285260915620833190121704385718897845374186418391149"
                            "9382298174300842802520571054770974715924354885364210719623692175321634302765"
                            "7297856345865465003098059554980535167165530164783787512766137631699199568264"
                            "2375816999170091743849513897976773894897600778259042036157870102542898205042"
                            "7824123812896846325727571132224667258507881896937642961571713505892072407975"
                            "4167887539957552951687481686900641734834741117188502891477234527429301627644"
                            "2794492875650714360227524636128081818630024032225101222200417262230400985652"
                            "6133918939976059064685467178150259360132780174492721790333309820809472782096"
                            "6669215745218497406398666491731060023940935402463537769599014346292359196777"
                            "4898777919808007536387191816929974055071243492867659427654725706983624979965"
                            "2588828114979227596312518313099294563104583211246004717935842828649410789415"
                            "3274149211810306460705786742724288676310502449572175876195924527963842129897"
                            "4056668226105102399224601207990825615048595034330043173576241906696162124872"
                            "3386233134850194648328344715313654134534996901866324678365697234270522511463"
                            "5789280376295646894522902528407610217018256991571974631546258135816088501166"
                            "8098782956142811004013054714739084379219712133036769522536250719667408076193"
                            "6593748551011415496691940551961194453787749398869477071600675694002153377006"
                            "4749034399961999229297501704000061861719748253039999907562947502610400000126"
                            "0505261328039999998442072148920400000001731030945643999999998287991372440000"
                            "0000014887031543999999999988794707440000000000007152314399999999999996235624"
                            "00000000000000156848999999999999999951490000000000000000009899999999999999999999");

    cout << "\nSUCCESS!\n";
}
