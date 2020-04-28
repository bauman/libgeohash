from pytest import raises
import pylibgeohash


def test_imported_functions():
    assert callable(pylibgeohash.from_hash)
    assert callable(pylibgeohash.to_hash)
    assert callable(pylibgeohash.geohash_decode)
    assert callable(pylibgeohash.geohash_encode)


def test_decoding():
    coord = pylibgeohash.geohash_decode("ezs42")
    assert isinstance(coord, dict)
    # oof floating point ==
    assert coord['latitude'] == 42.60498046875
    assert coord['longitude'] == -5.60302734375
    assert coord['north'] == 42.626953125
    assert coord['east'] == -5.5810546875
    assert coord['south'] == 42.5830078125
    assert coord['west'] == -5.625
    assert coord['dimension']['height'] == 0.0439453125
    assert coord['dimension']['width'] == 0.0439453125


def test_raises_on_bad_decode_param():
    with raises(TypeError):
        hash = pylibgeohash.geohash_encode(12)


def test_creats_coord_until_bad_char():
    coord = pylibgeohash.geohash_decode("ezs42-  ")
    # calculate 9xj5sm
    # stops calculating at - because "-" and space are not allowed chars
    assert coord['latitude'] == 42.60498046875
    assert coord['longitude'] == -5.60302734375
    assert coord['north'] == 42.626953125
    assert coord['east'] == -5.5810546875
    assert coord['south'] == 42.5830078125
    assert coord['west'] == -5.625
    assert coord['dimension']['height'] == 0.0439453125
    assert coord['dimension']['width'] == 0.0439453125


def test_encoding():
    hash = pylibgeohash.geohash_encode(40.018141, -105.274858, 1)
    assert hash == "9"
    hash = pylibgeohash.geohash_encode(40.018141, -105.274858, 2)
    assert hash == "9x"
    hash = pylibgeohash.geohash_encode(40.018141, -105.274858, 3)
    assert hash == "9xj"
    hash = pylibgeohash.geohash_encode(40.018141, -105.274858, 4)
    assert hash == "9xj5"
    hash = pylibgeohash.geohash_encode(40.018141, -105.274858, 5)
    assert hash == "9xj5s"
    hash = pylibgeohash.geohash_encode(40.018141, -105.274858, 6)
    assert hash == "9xj5sm"


def test_raises_on_bad_encode_param():
    with raises(TypeError):
        hash = pylibgeohash.geohash_encode("not_correct_param")


def test_raises_on_bad_encode_value():
    with raises(ValueError):
        hash = pylibgeohash.geohash_encode(40.018141, -105.274858, 0)
    with raises(ValueError):
        hash = pylibgeohash.geohash_encode(40.018141, -105.274858, 13)


def test_redefining_import_works():
    import pylibgeohash as geohash
    geohash.geohash_decode("ezs42")


def test_get_neighbors():
    neighbors = pylibgeohash.geohash_neighbors("ezs42")
    assert neighbors[0] == "ezs48"
    assert neighbors[1] == "ezs49"
    assert neighbors[2] == "ezs43"
    assert neighbors[3] == "ezs41"
    assert neighbors[4] == "ezs40"
    assert neighbors[5] == "ezefp"
    assert neighbors[6] == "ezefr"
    assert neighbors[7] == "ezefx"


def test_raises_on_bad_neighbor_call():
    with raises(ValueError):
        neighbors = pylibgeohash.geohash_neighbors("")


def test_raises_on_bad_neighbor_call_invalid_str():
    with raises(UnicodeDecodeError):
        neighbors = pylibgeohash.geohash_neighbors("☀")


def test_raises_on_bad_neighbor_call_empty_but_valid_str():
    with raises(ArithmeticError):
        neighbors = pylibgeohash.geohash_neighbors(" ")

