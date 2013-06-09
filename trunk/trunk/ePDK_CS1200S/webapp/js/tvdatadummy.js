    function GetEPGDummy() {
        // 1)extract ARTERMIS data guide and insert into _channels[]['channel'], _channels[]['programs'] & headers
        // 2)setup _headerStart, _headerEnd, _channelStart, _channelEnd

        _channels = new Array(10);

        for (var i = 0; i < _channels.length; i++)
            _channels[i] = new Object();

        _channels[0]['channel'] = { name: "BBC ONE", number: "1", id: "1043", img: "<img src=\"images/BBCOne.jpg\" height=\"16\" width=\"30\">"};
        _channels[1]['channel'] = { name: "BBC TWO", number: "2", id: "1044", img: "<img src=\"images/BBCTwo.jpg\" height=\"16\" width=\"30\">" };
        _channels[2]['channel'] = { name: "ITV1", number: "3", id: "1033", img: "<img src=\"images/ITV1.jpg\" height=\"16\" width=\"30\">" };
        _channels[3]['channel'] = { name: "Channel 4", number: "4", id: "1034", img: "<img src=\"images/Channel4.jpg\" height=\"16\" width=\"30\">" };
        _channels[4]['channel'] = { name: "Channel 5", number: "5", id: "1035", img: "<img src=\"images/Channel5.jpg\" height=\"16\" width=\"30\">" };
        _channels[5]['channel'] = { name: "Channel 6", number: "6", id: "1036", img: "<img src=\"images/Channel6.jpg\" height=\"16\" width=\"30\">" };
        _channels[6]['channel'] = { name: "Channel 7", number: "7", id: "1037", img: "<img src=\"images/Channel7.jpg\" height=\"16\" width=\"30\">" };
        _channels[7]['channel'] = { name: "Channel 8", number: "8", id: "1038", img: "<img src=\"images/Channel7.jpg\" height=\"16\" width=\"30\">" };
        _channels[8]['channel'] = { name: "Channel 9", number: "9", id: "1039", img: "<img src=\"images/Channel7.jpg\" height=\"16\" width=\"30\">" };
        _channels[9]['channel'] = { name: "Channel 10", number: "10", id: "1040", img: "" };

        _channels[0]['programs'] = new Array( 
            new Program("1043", "Little Robots", "test", 1815, 1830, 15),
            new Program("1044", "Hondo", "test", 1830, 1900, 30),
            new Program("1033", "X Factor", "test", 1900, 1930, 30),
            new Program("1034", "The Simpsons", "test", 1930, 2000, 30),
            new Program("1035", "Teenage Mutant", "test", 2000, 2030, 30),
            new Program("1036", "According To Jim", "test", 2030, 2100, 30)
        );

        _channels[1]['programs'] = new Array(
            new Program("1043", "Match of the Day Live, this is a long description test", "test", 1845, 1900, 15),
            new Program("1044", "Washington Journal", "test", 1900, 1915, 15),
            new Program("1033", "Leadership File", "test", 1915, 2000, 45),
            new Program("1034", "In the Bag", "test", 2000, 2010, 10),
            new Program("1035", "Weekend Punjabi", "test", 2010, 2030, 20)
        );

        _channels[2]['programs'] = new Array(
            new Program("1043", "Wizards", "test", 1800, 1815, 15),
            new Program("1044", "Suite Life", "test", 1820, 1900, 40),
            new Program("1033", "Phil of the Future", "test", 1900, 1930, 30),
            new Program("1034", "Spongebob", "test", 1930, 1945, 15),
            new Program("1035", "Batman", "test", 1945, 2015, 30)
        );

        _channels[3]['programs'] = new Array(
            new Program("1043", "Benny Hill", "test", 1800, 1830, 30),
            new Program("1044", "Lost In Space", "test", 1830, 1900, 30),
            new Program("1033", "X Files", "test", 1900, 1930, 30),
            new Program("1034", "The Simpsons", "test", 1930, 2000, 30),
            new Program("1035", "Teenage Mutant", "test", 2000, 2030, 30)
        );

        _channels[4]['programs'] = new Array(
            new Program("1043", "Angel", "test", 1800, 1830, 30),
            new Program("1044", "Football Live", "test", 1830, 1900, 30),
            new Program("1033", "Max Factor", "test", 1900, 1930, 30),
            new Program("1034", "The Who", "test", 1930, 2000, 30),
            new Program("1035", "Teenage", "test", 2000, 2030, 30)
        );

        _channels[5]['programs'] = new Array(
            new Program("61", "6.1", "test", 1800, 1830, 30),
            new Program("62", "6.2", "test", 1830, 1900, 30),
            new Program("63", "6.3", "test", 1900, 1930, 30),
            new Program("64", "6.4", "test", 1930, 2000, 30),
            new Program("65", "6.5", "test", 2000, 2030, 30)
        );

        _channels[6]['programs'] = new Array(
            new Program("71", "7.1", "test", 1800, 1830, 30),
            new Program("72", "7.2", "test", 1830, 1900, 30),
            new Program("73", "7.3", "test", 1900, 1930, 30),
            new Program("74", "7.4", "test", 1930, 2000, 30),
            new Program("75", "7.5", "test", 2000, 2030, 30)
        );

        _channels[7]['programs'] = new Array(
            new Program("81", "8.1", "test", 1800, 1830, 30),
            new Program("82", "8.2", "test", 1830, 1900, 30),
            new Program("83", "8.3", "test", 1900, 1930, 30),
            new Program("84", "8.4", "test", 1930, 2000, 30),
            new Program("85", "8.5", "test", 2000, 2030, 30)
        );

        _channels[8]['programs'] = new Array(
            new Program("91", "9.1", "test", 1800, 1830, 30),
            new Program("92", "9.2", "test", 1830, 1900, 30),
            new Program("93", "9.3", "test", 1900, 1930, 30),
            new Program("94", "9.4", "test", 1930, 2000, 30),
            new Program("95", "9.5", "test", 2000, 2030, 30)
        );

        _channels[9]['programs'] = new Array(
            new Program("10-1", "10.1", "test", 1800, 1830, 30),
            new Program("10-2", "10.2", "test", 1830, 1900, 30),
            new Program("10-3", "10.3", "test", 1900, 1930, 30),
            new Program("10-4", "10.4", "test", 1930, 2000, 30),
            new Program("10-5", "10.5", "test", 2000, 2030, 30)
        );

        _headers = new Array(
            new Header("6:00 PM", 1800, 1830, 30),
            new Header("6:30 PM", 1830, 1900, 30),
            new Header("7:00 PM", 1900, 1930, 30),
            new Header("7:30 PM", 1930, 2000, 30),
            new Header("8:00 PM", 2000, 2030, 30),

            new Header("8:30 PM", 2030, 2100, 30),
            new Header("9:00 PM", 2100, 2130, 30),
            new Header("9:30 PM", 2130, 2200, 30),
            new Header("10:00 PM", 2200, 2230, 30),
            new Header("10:30 PM", 2230, 2300, 30)
            );

        for (var i = 0; i < _channels.length; i++)
        {
            if (_channels[i]['channel'].name == "BBC ONE")
                _channels[i]['channel'].img = "<img src=\"images/BBCOne.jpg\" height=\"16\" width=\"30\">";
            else if (_channels[i]['channel'].name == "BBC TWO")
                _channels[i]['channel'].img = "<img src=\"images/BBCTwo.jpg\" height=\"16\" width=\"30\">";
            else if (_channels[i]['channel'].name == "ITV1")
                _channels[i]['channel'].img = "<img src=\"images/ITV1.jpg\" height=\"16\" width=\"30\">";
            else if (_channels[i]['channel'].name == "Channel 4")
                _channels[i]['channel'].img = "<img src=\"images/Channel4.jpg\" height=\"16\" width=\"30\">";
            else if (_channels[i]['channel'].name == "Channel 5")
                _channels[i]['channel'].img = "<img src=\"images/Channel5.jpg\" height=\"16\" width=\"30\">";
        }
    }
