import json
import os

years_data = {
    2080: [
        {
            "month": 1, "day": 1, "isPublicHoliday": True,
            "titleNP": "नयाँ वर्ष (Nepali New Year)",
            "titleEN": "Nepali New Year 2080 & Bisket Jatra",
            "category": "PUBLIC HOLIDAY",
            "description": "National public holiday marking the official Bikram Sambat 2080 New Year."
        },
        {
            "month": 1, "day": 11, "isPublicHoliday": False,
            "titleNP": "लोकतन्त्र दिवस",
            "titleEN": "Loktantra Diwas (Democracy Day)",
            "category": "NATIONAL DAY",
            "description": "Commemorating the historic 2063 restoration of the democratic parliament in Nepal."
        },
        {
            "month": 1, "day": 18, "isPublicHoliday": True,
            "titleNP": "अन्तर्राष्ट्रिय श्रमिक दिवस",
            "titleEN": "International Labour Day (May 1)",
            "category": "PUBLIC HOLIDAY",
            "description": "Global public holiday celebrating the labour movement and workers' solidarity."
        },
        {
            "month": 1, "day": 22, "isPublicHoliday": True,
            "titleNP": "बुद्ध जयन्ती / उभौली पर्व",
            "titleEN": "Buddha Jayanti & Ubhauli Parva",
            "category": "PUBLIC HOLIDAY",
            "description": "Celebrating the birth, enlightenment, and Mahaparinirvana of Gautama Buddha and Kirat Ubhauli."
        },
        {
            "month": 2, "day": 15, "isPublicHoliday": True,
            "titleNP": "गणतन्त्र दिवस",
            "titleEN": "Republic Day (Ganatantra Diwas)",
            "category": "PUBLIC HOLIDAY",
            "description": "National holiday commemorating Nepal's declaration as a Federal Democratic Republic."
        },
        {
            "month": 2, "day": 22, "isPublicHoliday": False,
            "titleNP": "विश्व वातावरण दिवस",
            "titleEN": "World Environment Day (June 5)",
            "category": "GLOBAL DAY",
            "description": "Global advocacy for ecological balance, climate action, and biodiversity protection."
        },
        {
            "month": 3, "day": 15, "isPublicHoliday": False,
            "titleNP": "राष्ट्रिय धान दिवस / दही चिउरा",
            "titleEN": "National Paddy Day (Dahi Chiura)",
            "category": "FESTIVAL",
            "description": "Agrarian monsoon celebration marking the rice plantation season with beaten rice and yogurt."
        },
        {
            "month": 3, "day": 18, "isPublicHoliday": False,
            "titleNP": "गुरु पूर्णिमा",
            "titleEN": "Guru Purnima",
            "category": "FESTIVAL",
            "description": "Auspicious day dedicated to expressing reverence and gratitude to teachers and spiritual mentors."
        },
        {
            "month": 4, "day": 1, "isPublicHoliday": False,
            "titleNP": "साउने सङ्क्रान्ति",
            "titleEN": "Saune Sankranti",
            "category": "FESTIVAL",
            "description": "First day of Shrawan marking Dakshinayana transition, holy dips, and Luta Falne rituals."
        },
        {
            "month": 4, "day": 15, "isPublicHoliday": False,
            "titleNP": "खीर खाने दिन",
            "titleEN": "Kheer Khane Din",
            "category": "CULTURAL EVENT",
            "description": "Mid-monsoon culinary tradition of preparing, offering, and relishing rich sweet rice pudding."
        },
        {
            "month": 5, "day": 14, "isPublicHoliday": True,
            "titleNP": "जनै पूर्णिमा / रक्षाबन्धन",
            "titleEN": "Janai Purnima & Rakshya Bandhan",
            "category": "PUBLIC HOLIDAY",
            "description": "Renewal of the sacred thread (Janai) and tying protective Rakhi wristbands."
        },
        {
            "month": 5, "day": 15, "isPublicHoliday": False,
            "titleNP": "गाईजात्रा (Gai Jatra)",
            "titleEN": "Gai Jatra (Cow Carnival)",
            "category": "FESTIVAL",
            "description": "Historic Newar festival commemorating deceased family souls through satire, laughter, and street parades."
        },
        {
            "month": 5, "day": 20, "isPublicHoliday": True,
            "titleNP": "श्रीकृष्ण जन्माष्टमी",
            "titleEN": "Shree Krishna Janmashtami",
            "category": "PUBLIC HOLIDAY",
            "description": "Grand celebrations and midnight prayers honoring the birth appearance of Lord Krishna."
        },
        {
            "month": 5, "day": 21, "isPublicHoliday": True,
            "titleNP": "गौरा पर्व",
            "titleEN": "Gaura Parva",
            "category": "PUBLIC HOLIDAY",
            "description": "Celebration of Goddess Gauri and Shiva in western Nepal with resonant Deuda dance."
        },
        {
            "month": 5, "day": 22, "isPublicHoliday": False,
            "titleNP": "निजामती सेवा दिवस",
            "titleEN": "Civil Service Day",
            "category": "NATIONAL DAY",
            "description": "Commemorating national public administration and honoring meritorious civil servants."
        },
        {
            "month": 6, "day": 1, "isPublicHoliday": True,
            "titleNP": "हरितालिका तीज",
            "titleEN": "Haritalika Teej",
            "category": "PUBLIC HOLIDAY",
            "description": "Joyous Hindu women's fasting and cultural festival celebrated with red sarees, songs, and temple darshan."
        },
        {
            "month": 6, "day": 3, "isPublicHoliday": True,
            "titleNP": "संविधान दिवस",
            "titleEN": "Constitution Day (National Day)",
            "category": "PUBLIC HOLIDAY",
            "description": "National day marking the historic promulgation of the Constitution of Nepal in 2072 BS."
        },
        {
            "month": 6, "day": 11, "isPublicHoliday": True,
            "titleNP": "इन्द्रजात्रा (Indra Jatra)",
            "titleEN": "Indra Jatra & Kumari Yatra",
            "category": "PUBLIC HOLIDAY",
            "description": "Kathmandu Valley's premier chariot festival honoring Living Goddess Kumari and Lord Indra."
        },
        {
            "month": 6, "day": 28, "isPublicHoliday": True,
            "titleNP": "घटस्थापना (Ghatasthapana)",
            "titleEN": "Ghatasthapana (Dashain Begins)",
            "category": "PUBLIC HOLIDAY",
            "description": "Inauguration of Navaratri with sacred Kalash installation and planting of auspicious golden Jamara."
        },
        {
            "month": 7, "day": 4, "isPublicHoliday": True,
            "titleNP": "फूलपाती (Fulpati)",
            "titleEN": "Fulpati (Maha Saptami)",
            "category": "PUBLIC HOLIDAY",
            "description": "Royal palanquin arrival of holy floral foliage and auspicious plants dedicated to Durga Bhavani."
        },
        {
            "month": 7, "day": 5, "isPublicHoliday": True,
            "titleNP": "महाअष्टमी (Maha Ashtami)",
            "titleEN": "Maha Ashtami & Kalratri",
            "category": "PUBLIC HOLIDAY",
            "description": "Deep spiritual night of powerful Durga veneration and vehicle sanctification."
        },
        {
            "month": 7, "day": 6, "isPublicHoliday": True,
            "titleNP": "महानवमी (Maha Navami)",
            "titleEN": "Maha Navami (Navami Puja)",
            "category": "PUBLIC HOLIDAY",
            "description": "Worship of divine craftsmanship tools, engines, machinery, and Taleju Bhawani temple opening."
        },
        {
            "month": 7, "day": 7, "isPublicHoliday": True,
            "titleNP": "विजया दशमी (Bada Dashain)",
            "titleEN": "Vijaya Dashami (Tika Day)",
            "category": "PUBLIC HOLIDAY",
            "description": "Grand national day of receiving red vermilion Tika, golden Jamara, and blessings from family elders."
        },
        {
            "month": 7, "day": 8, "isPublicHoliday": True,
            "titleNP": "दशैं एकादशी",
            "titleEN": "Dashain Ekadashi",
            "category": "PUBLIC HOLIDAY",
            "description": "Continuation of extended family gatherings, festive delicacies, and traditional bamboo swings (Ping)."
        },
        {
            "month": 7, "day": 24, "isPublicHoliday": False,
            "titleNP": "काग तिहार (Kag Tihar)",
            "titleEN": "Kag Tihar (Crow Worship)",
            "category": "FESTIVAL",
            "description": "First day of Yamapanchak honoring crows as messenger birds of nature and health."
        },
        {
            "month": 7, "day": 25, "isPublicHoliday": False,
            "titleNP": "कुकुर तिहार (Kukur Tihar)",
            "titleEN": "Kukur Tihar (Dog Worship)",
            "category": "FESTIVAL",
            "description": "Honoring humanity's loyal canine companions with flower garlands and rich meals."
        },
        {
            "month": 7, "day": 26, "isPublicHoliday": True,
            "titleNP": "लक्ष्मी पूजा (Laxmi Puja)",
            "titleEN": "Laxmi Puja & Deepawali",
            "category": "PUBLIC HOLIDAY",
            "description": "Illuminating homes with oil lamps, Rangoli artwork, and inviting Goddess Laxmi's prosperity."
        },
        {
            "month": 7, "day": 28, "isPublicHoliday": True,
            "titleNP": "गोवर्धन पूजा / म्ह पूजा",
            "titleEN": "Govardhan Puja & Mha Puja",
            "category": "PUBLIC HOLIDAY",
            "description": "Newar New Year (Nepal Sambat 1144), soul self-worship (Mha Puja), and Govardhan veneration."
        },
        {
            "month": 7, "day": 29, "isPublicHoliday": True,
            "titleNP": "भाइटीका (Bhai Tika)",
            "titleEN": "Bhai Tika (Tihar Finale)",
            "category": "PUBLIC HOLIDAY",
            "description": "Sisters adorn brothers with rainbow Saptarangi Tika and Makhamali garlands for long life and prosperity."
        },
        {
            "month": 8, "day": 3, "isPublicHoliday": True,
            "titleNP": "छठ पर्व (Chhath Parva)",
            "titleEN": "Chhath Parva (Sun Festival)",
            "category": "PUBLIC HOLIDAY",
            "description": "Ancient Vedic riverbank celebrations offering Arghya to the setting and rising Sun God."
        },
        {
            "month": 8, "day": 25, "isPublicHoliday": False,
            "titleNP": "बाला चतुर्दशी",
            "titleEN": "Bala Chaturdashi",
            "category": "FESTIVAL",
            "description": "Sowing sacred Satbij (seven grains) at holy shrines for peaceful journey of ancestors."
        },
        {
            "month": 9, "day": 9, "isPublicHoliday": True,
            "titleNP": "क्रिसमस डे (Christmas)",
            "titleEN": "Christmas Day (Dec 25)",
            "category": "PUBLIC HOLIDAY",
            "description": "Global Christmas holiday celebrating goodwill, love, and the nativity of Jesus Christ."
        },
        {
            "month": 9, "day": 10, "isPublicHoliday": True,
            "titleNP": "उधौली पर्व / योमरी पुन्हि",
            "titleEN": "Udhauli & Yomari Punhi",
            "category": "PUBLIC HOLIDAY",
            "description": "Kirat winter harvest thanksgiving and Newar sweet steamed Yomari dumpling celebration."
        },
        {
            "month": 9, "day": 15, "isPublicHoliday": True,
            "titleNP": "तमु ल्होसार",
            "titleEN": "Tamu Lhosar (Gurung New Year)",
            "category": "PUBLIC HOLIDAY",
            "description": "Gurung community New Year festival celebrated with colorful traditional attire and feasts."
        },
        {
            "month": 9, "day": 27, "isPublicHoliday": True,
            "titleNP": "राष्ट्रिय एकता दिवस / पृथ्वी जयन्ती",
            "titleEN": "National Unity Day (Prithvi Jayanti)",
            "category": "PUBLIC HOLIDAY",
            "description": "Commemorating King Prithvi Narayan Shah, architect of modern unified Nepal."
        },
        {
            "month": 10, "day": 1, "isPublicHoliday": True,
            "titleNP": "माघे सङ्क्रान्ति / माघी",
            "titleEN": "Maghe Sankranti & Maghi",
            "category": "PUBLIC HOLIDAY",
            "description": "Winter solstice celebration with molasses (Chaku), ghee, yam, and Tharu New Year."
        },
        {
            "month": 10, "day": 16, "isPublicHoliday": False,
            "titleNP": "सहिद दिवस",
            "titleEN": "Martyrs' Day (Sahid Diwas)",
            "category": "NATIONAL DAY",
            "description": "Solemn national tribute honoring the brave martyrs who sacrificed lives for national freedom."
        },
        {
            "month": 10, "day": 27, "isPublicHoliday": True,
            "titleNP": "सोनाम ल्होसार",
            "titleEN": "Sonam Lhosar (Tamang New Year)",
            "category": "PUBLIC HOLIDAY",
            "description": "Tamang New Year celebrated with monastery prayers, mask dances, and joyful festivities."
        },
        {
            "month": 10, "day": 30, "isPublicHoliday": False,
            "titleNP": "सरस्वती पूजा / वसन्त पञ्चमी",
            "titleEN": "Saraswati Puja (Vasant Panchami)",
            "category": "FESTIVAL",
            "description": "Veneration of Goddess Saraswati, patron of wisdom, learning, music, and start of springtime."
        },
        {
            "month": 11, "day": 7, "isPublicHoliday": True,
            "titleNP": "राष्ट्रिय प्रजातन्त्र दिवस",
            "titleEN": "National Democracy Day",
            "category": "PUBLIC HOLIDAY",
            "description": "Commemorating the historic 2007 BS establishment of democratic governance in Nepal."
        },
        {
            "month": 11, "day": 24, "isPublicHoliday": True,
            "titleNP": "अन्तर्राष्ट्रिय महिला दिवस",
            "titleEN": "International Women's Day (Mar 8)",
            "category": "PUBLIC HOLIDAY",
            "description": "Global observance honoring women's empowerment, rights, leadership, and equality."
        },
        {
            "month": 11, "day": 25, "isPublicHoliday": True,
            "titleNP": "महाशिवरात्रि",
            "titleEN": "Maha Shivaratri",
            "category": "PUBLIC HOLIDAY",
            "description": "Auspicious cosmic night of Lord Shiva celebrated by millions of devotees at Pashupatinath."
        },
        {
            "month": 11, "day": 28, "isPublicHoliday": True,
            "titleNP": "ग्याल्पो ल्होसार",
            "titleEN": "Gyalpo Lhosar (Sherpa New Year)",
            "category": "PUBLIC HOLIDAY",
            "description": "Tibetan and Sherpa New Year marked with sacred rituals, Guthuk soup, and Losar joy."
        },
        {
            "month": 12, "day": 11, "isPublicHoliday": True,
            "titleNP": "फागु पूर्णिमा (Holi)",
            "titleEN": "Holi (Festival of Colors)",
            "category": "PUBLIC HOLIDAY",
            "description": "Vibrant and euphoric spring carnival of colorful powders, joy, water splashes, and friendship."
        },
        {
            "month": 12, "day": 26, "isPublicHoliday": False,
            "titleNP": "घोडेजात्रा (Ghode Jatra)",
            "titleEN": "Ghode Jatra (Horse Festival)",
            "category": "CULTURAL EVENT",
            "description": "Spectacular annual equestrian parade and horse racing carnival in Kathmandu valley."
        }
    ],

    2081: [
        {
            "month": 1, "day": 1, "isPublicHoliday": True,
            "titleNP": "नयाँ वर्ष (Nepali New Year)",
            "titleEN": "Nepali New Year 2081 & Bisket Jatra",
            "category": "PUBLIC HOLIDAY",
            "description": "Official national public holiday welcoming the Bikram Sambat 2081 New Year."
        },
        {
            "month": 1, "day": 11, "isPublicHoliday": False,
            "titleNP": "लोकतन्त्र दिवस",
            "titleEN": "Loktantra Diwas (Democracy Day)",
            "category": "NATIONAL DAY",
            "description": "Commemorating the restoration of democratic institutions in Nepal in 2063 BS."
        },
        {
            "month": 1, "day": 19, "isPublicHoliday": True,
            "titleNP": "अन्तर्राष्ट्रिय श्रमिक दिवस",
            "titleEN": "International Labour Day (May 1)",
            "category": "PUBLIC HOLIDAY",
            "description": "International May Day holiday honoring working men and women worldwide."
        },
        {
            "month": 2, "day": 10, "isPublicHoliday": True,
            "titleNP": "बुद्ध जयन्ती / उभौली पर्व",
            "titleEN": "Buddha Jayanti & Ubhauli Parva",
            "category": "PUBLIC HOLIDAY",
            "description": "Triple blessed day honoring Gautama Buddha's birth, bodhi enlightenment, and nirvana."
        },
        {
            "month": 2, "day": 15, "isPublicHoliday": True,
            "titleNP": "गणतन्त्र दिवस",
            "titleEN": "Republic Day (Ganatantra Diwas)",
            "category": "PUBLIC HOLIDAY",
            "description": "Celebrating the declaration of Nepal as a sovereign Federal Democratic Republic."
        },
        {
            "month": 2, "day": 23, "isPublicHoliday": False,
            "titleNP": "विश्व वातावरण दिवस",
            "titleEN": "World Environment Day (June 5)",
            "category": "GLOBAL DAY",
            "description": "Worldwide UN observance for planetary conservation and climate resilience."
        },
        {
            "month": 3, "day": 15, "isPublicHoliday": False,
            "titleNP": "राष्ट्रिय धान दिवस / दही चिउरा",
            "titleEN": "National Paddy Day (Asar 15)",
            "category": "FESTIVAL",
            "description": "National agrarian festival celebrating rice planting in monsoon mud with curd and beaten rice."
        },
        {
            "month": 3, "day": 27, "isPublicHoliday": False,
            "titleNP": "गुरु पूर्णिमा",
            "titleEN": "Guru Purnima",
            "category": "FESTIVAL",
            "description": "Spiritual day devoted to expressing heartfelt gratitude to teachers and gurus."
        },
        {
            "month": 4, "day": 1, "isPublicHoliday": False,
            "titleNP": "साउने सङ्क्रान्ति",
            "titleEN": "Saune Sankranti",
            "category": "FESTIVAL",
            "description": "Sun enters Cancer (Dakshinayana), opening the pious monsoon month of Shrawan."
        },
        {
            "month": 4, "day": 15, "isPublicHoliday": False,
            "titleNP": "खीर खाने दिन",
            "titleEN": "Kheer Khane Din",
            "category": "CULTURAL EVENT",
            "description": "Traditional mid-monsoon feast of rich aromatic milk pudding with family."
        },
        {
            "month": 5, "day": 3, "isPublicHoliday": True,
            "titleNP": "जनै पूर्णिमा / रक्षाबन्धन",
            "titleEN": "Janai Purnima & Rakshya Bandhan",
            "category": "PUBLIC HOLIDAY",
            "description": "Sacred thread changing ceremony, tying Rakhi bonds, and relishing Kwati soup."
        },
        {
            "month": 5, "day": 4, "isPublicHoliday": False,
            "titleNP": "गाईजात्रा (Gai Jatra)",
            "titleEN": "Gai Jatra (Cow Carnival)",
            "category": "FESTIVAL",
            "description": "Kathmandu Valley street carnival celebrating memories of lost loved ones with humor."
        },
        {
            "month": 5, "day": 10, "isPublicHoliday": True,
            "titleNP": "श्रीकृष्ण जन्माष्टमी / गौरा पर्व",
            "titleEN": "Krishna Janmashtami & Gaura Parva",
            "category": "PUBLIC HOLIDAY",
            "description": "Celebration of Lord Krishna's divine birth alongside western Nepal's Gaura festival."
        },
        {
            "month": 5, "day": 21, "isPublicHoliday": True,
            "titleNP": "हरितालिका तीज",
            "titleEN": "Haritalika Teej",
            "category": "PUBLIC HOLIDAY",
            "description": "Major festival of Hindu women featuring devotion, colorful traditional attire, and songs."
        },
        {
            "month": 5, "day": 22, "isPublicHoliday": False,
            "titleNP": "निजामती सेवा दिवस",
            "titleEN": "Civil Service Day",
            "category": "NATIONAL DAY",
            "description": "Annual observance commemorating dedication to good governance and public service."
        },
        {
            "month": 5, "day": 23, "isPublicHoliday": False,
            "titleNP": "ऋषि पञ्चमी",
            "titleEN": "Rishi Panchami",
            "category": "FESTIVAL",
            "description": "Veneration of the Sapta Rishis for spiritual cleansing and purity."
        },
        {
            "month": 6, "day": 1, "isPublicHoliday": True,
            "titleNP": "इन्द्रजात्रा (Indra Jatra)",
            "titleEN": "Indra Jatra & Kumari Yatra",
            "category": "PUBLIC HOLIDAY",
            "description": "Grand festival honoring Living Goddess Kumari and rain deity Lord Indra."
        },
        {
            "month": 6, "day": 3, "isPublicHoliday": True,
            "titleNP": "संविधान दिवस",
            "titleEN": "Constitution Day (National Day)",
            "category": "PUBLIC HOLIDAY",
            "description": "Commemorating the adoption of Nepal's democratic Constitution in 2072 BS."
        },
        {
            "month": 6, "day": 17, "isPublicHoliday": True,
            "titleNP": "घटस्थापना (Ghatasthapana)",
            "titleEN": "Ghatasthapana (Dashain Begins)",
            "category": "PUBLIC HOLIDAY",
            "description": "Commencement of Bada Dashain with the installation of holy Kalash and Jamara."
        },
        {
            "month": 6, "day": 24, "isPublicHoliday": True,
            "titleNP": "फूलपाती (Fulpati)",
            "titleEN": "Fulpati (Maha Saptami)",
            "category": "PUBLIC HOLIDAY",
            "description": "Ceremonial procession bringing auspicious sacred plant offerings to Durbar squares."
        },
        {
            "month": 6, "day": 25, "isPublicHoliday": True,
            "titleNP": "महाअष्टमी (Maha Ashtami)",
            "titleEN": "Maha Ashtami",
            "category": "PUBLIC HOLIDAY",
            "description": "Spiritual night of supreme devotion to Goddess Durga Bhavani."
        },
        {
            "month": 6, "day": 26, "isPublicHoliday": True,
            "titleNP": "महानवमी (Maha Navami)",
            "titleEN": "Maha Navami",
            "category": "PUBLIC HOLIDAY",
            "description": "Worship of implements, vehicles, and instruments of craftsmanship and protection."
        },
        {
            "month": 6, "day": 27, "isPublicHoliday": True,
            "titleNP": "विजया दशमी (Bada Dashain)",
            "titleEN": "Vijaya Dashami (Bada Dashain)",
            "category": "PUBLIC HOLIDAY",
            "description": "Main festival day of receiving auspicious red Tika, golden Jamara, and elder blessings."
        },
        {
            "month": 6, "day": 28, "isPublicHoliday": True,
            "titleNP": "दशैं एकादशी (Papankusha)",
            "titleEN": "Dashain Ekadashi",
            "category": "PUBLIC HOLIDAY",
            "description": "Continuing family visits, bamboo ping rides, and joyous Dashain celebrations."
        },
        {
            "month": 7, "day": 13, "isPublicHoliday": False,
            "titleNP": "काग तिहार (Kag Tihar)",
            "titleEN": "Kag Tihar (Crow Worship)",
            "category": "FESTIVAL",
            "description": "Honoring crows as nature's messengers in the opening day of Yamapanchak."
        },
        {
            "month": 7, "day": 14, "isPublicHoliday": False,
            "titleNP": "कुकुर तिहार (Kukur Tihar)",
            "titleEN": "Kukur Tihar (Dog Worship)",
            "category": "FESTIVAL",
            "description": "Celebrating the bond of loyalty and friendship with dogs through garlands and food."
        },
        {
            "month": 7, "day": 15, "isPublicHoliday": True,
            "titleNP": "लक्ष्मी पूजा (Laxmi Puja)",
            "titleEN": "Laxmi Puja & Deepawali",
            "category": "PUBLIC HOLIDAY",
            "description": "Illuminating houses with sparkling oil lamps, welcoming Goddess Laxmi's wealth."
        },
        {
            "month": 7, "day": 17, "isPublicHoliday": True,
            "titleNP": "गोवर्धन पूजा / म्ह पूजा",
            "titleEN": "Govardhan Puja & Nepal Sambat",
            "category": "PUBLIC HOLIDAY",
            "description": "Nepal Sambat 1145 New Year, Mha Puja self-purification, and Govardhan veneration."
        },
        {
            "month": 7, "day": 18, "isPublicHoliday": True,
            "titleNP": "भाइटीका (Bhai Tika)",
            "titleEN": "Bhai Tika (Tihar Finale)",
            "category": "PUBLIC HOLIDAY",
            "description": "Sisters apply seven-colored Tika to brothers, praying for their long life and happiness."
        },
        {
            "month": 7, "day": 22, "isPublicHoliday": True,
            "titleNP": "छठ पर्व (Chhath Parva)",
            "titleEN": "Chhath Parva (Sun God Festival)",
            "category": "PUBLIC HOLIDAY",
            "description": "Sacred riverbank rituals offering Arghya to the setting and rising Surya."
        },
        {
            "month": 8, "day": 14, "isPublicHoliday": False,
            "titleNP": "बाला चतुर्दशी",
            "titleEN": "Bala Chaturdashi",
            "category": "FESTIVAL",
            "description": "Sowing Satbij at Mrigasthali forest in Pashupatinath for eternal peace of departed souls."
        },
        {
            "month": 8, "day": 21, "isPublicHoliday": False,
            "titleNP": "विवाह पञ्चमी",
            "titleEN": "Vivah Panchami",
            "category": "FESTIVAL",
            "description": "Commemorating the celestial wedding of Lord Ram and Goddess Sita in Janakpurdham."
        },
        {
            "month": 8, "day": 30, "isPublicHoliday": True,
            "titleNP": "उधौली पर्व / योमरी पुन्हि",
            "titleEN": "Udhauli Parva & Yomari Punhi",
            "category": "PUBLIC HOLIDAY",
            "description": "Kirat harvest celebration and Newar festive steamed sweet Yomari pastries."
        },
        {
            "month": 9, "day": 10, "isPublicHoliday": True,
            "titleNP": "क्रिसमस डे (Christmas)",
            "titleEN": "Christmas Day (Dec 25)",
            "category": "PUBLIC HOLIDAY",
            "description": "Global festival of peace, love, charity, and celebrating the birth of Jesus Christ."
        },
        {
            "month": 9, "day": 15, "isPublicHoliday": True,
            "titleNP": "तमु ल्होसार",
            "titleEN": "Tamu Lhosar (Gurung New Year)",
            "category": "PUBLIC HOLIDAY",
            "description": "Gurung community New Year festival marked with cultural dances and festivities."
        },
        {
            "month": 9, "day": 27, "isPublicHoliday": True,
            "titleNP": "राष्ट्रिय एकता दिवस / पृथ्वी जयन्ती",
            "titleEN": "National Unity Day (Prithvi Jayanti)",
            "category": "PUBLIC HOLIDAY",
            "description": "Honoring King Prithvi Narayan Shah, founder of unified modern Nepal."
        },
        {
            "month": 10, "day": 1, "isPublicHoliday": True,
            "titleNP": "माघे सङ्क्रान्ति / माघी",
            "titleEN": "Maghe Sankranti & Maghi",
            "category": "PUBLIC HOLIDAY",
            "description": "Winter festival of Tilko Laddu, Chaku, Ghee, and Tharu community New Year."
        },
        {
            "month": 10, "day": 16, "isPublicHoliday": False,
            "titleNP": "सहिद दिवस",
            "titleEN": "Martyrs' Day (Sahid Diwas)",
            "category": "NATIONAL DAY",
            "description": "Tribute to Nepal's courageous martyrs who laid down lives for democracy."
        },
        {
            "month": 10, "day": 16, "isPublicHoliday": True,
            "titleNP": "सोनाम ल्होसार",
            "titleEN": "Sonam Lhosar (Tamang New Year)",
            "category": "PUBLIC HOLIDAY",
            "description": "Tamang community New Year celebrated with monastery rituals and celebrations."
        },
        {
            "month": 10, "day": 21, "isPublicHoliday": False,
            "titleNP": "सरस्वती पूजा / वसन्त पञ्चमी",
            "titleEN": "Saraswati Puja (Vasant Panchami)",
            "category": "FESTIVAL",
            "description": "Worship of Goddess Saraswati, patron of wisdom, learning, music, and fine arts."
        },
        {
            "month": 11, "day": 7, "isPublicHoliday": True,
            "titleNP": "राष्ट्रिय प्रजातन्त्र दिवस",
            "titleEN": "National Democracy Day",
            "category": "PUBLIC HOLIDAY",
            "description": "Commemorating the overthrow of autocratic regime and birth of democracy in 2007 BS."
        },
        {
            "month": 11, "day": 14, "isPublicHoliday": True,
            "titleNP": "महाशिवरात्रि",
            "titleEN": "Maha Shivaratri",
            "category": "PUBLIC HOLIDAY",
            "description": "Grand festival night of Lord Shiva celebrated at Pashupatinath and temples countrywide."
        },
        {
            "month": 11, "day": 16, "isPublicHoliday": True,
            "titleNP": "ग्याल्पो ल्होसार",
            "titleEN": "Gyalpo Lhosar (Sherpa New Year)",
            "category": "PUBLIC HOLIDAY",
            "description": "Sherpa and Tibetan New Year welcoming the Year of the Wood Snake with Losar joy."
        },
        {
            "month": 11, "day": 24, "isPublicHoliday": True,
            "titleNP": "अन्तर्राष्ट्रिय महिला दिवस",
            "titleEN": "International Women's Day (Mar 8)",
            "category": "PUBLIC HOLIDAY",
            "description": "Celebrating women's social, economic, cultural, and political accomplishments."
        },
        {
            "month": 11, "day": 29, "isPublicHoliday": True,
            "titleNP": "फागु पूर्णिमा (Holi)",
            "titleEN": "Holi (Festival of Colors)",
            "category": "PUBLIC HOLIDAY",
            "description": "Joyful carnival of colors celebrating spring, camaraderie, and victory of good over evil."
        },
        {
            "month": 12, "day": 15, "isPublicHoliday": False,
            "titleNP": "घोडेजात्रा (Ghode Jatra)",
            "titleEN": "Ghode Jatra (Horse Festival)",
            "category": "CULTURAL EVENT",
            "description": "Spectacular equestrian stunts and horse parades at Tundikhel, Kathmandu."
        },
        {
            "month": 12, "day": 23, "isPublicHoliday": False,
            "titleNP": "चैते दशैं",
            "titleEN": "Chaite Dashain",
            "category": "FESTIVAL",
            "description": "Springtime festival worshipping Goddess Durga Bhavani and divine power."
        },
        {
            "month": 12, "day": 24, "isPublicHoliday": False,
            "titleNP": "राम नवमी",
            "titleEN": "Ram Navami",
            "category": "FESTIVAL",
            "description": "Auspicious appearance day of Lord Ram, symbol of righteousness and virtue."
        }
    ],

    2082: [
        {
            "month": 1, "day": 1, "isPublicHoliday": True,
            "titleNP": "नयाँ वर्ष (Nepali New Year)",
            "titleEN": "Nepali New Year 2082 & Bisket Jatra",
            "category": "PUBLIC HOLIDAY",
            "description": "Official national public holiday welcoming Bikram Sambat 2082 New Year."
        },
        {
            "month": 1, "day": 11, "isPublicHoliday": False,
            "titleNP": "लोकतन्त्र दिवस",
            "titleEN": "Loktantra Diwas (Democracy Day)",
            "category": "NATIONAL DAY",
            "description": "Commemorating the triumph of the historic 2063 People's Movement."
        },
        {
            "month": 1, "day": 18, "isPublicHoliday": True,
            "titleNP": "अन्तर्राष्ट्रिय श्रमिक दिवस",
            "titleEN": "International Labour Day (May 1)",
            "category": "PUBLIC HOLIDAY",
            "description": "Honoring workers' rights and solidarity across the world."
        },
        {
            "month": 1, "day": 29, "isPublicHoliday": True,
            "titleNP": "बुद्ध जयन्ती / उभौली पर्व",
            "titleEN": "Buddha Jayanti & Ubhauli Parva",
            "category": "PUBLIC HOLIDAY",
            "description": "Celebrating the birth, bodhi enlightenment, and Mahaparinirvana of Gautama Buddha."
        },
        {
            "month": 2, "day": 15, "isPublicHoliday": True,
            "titleNP": "गणतन्त्र दिवस",
            "titleEN": "Republic Day (Ganatantra Diwas)",
            "category": "PUBLIC HOLIDAY",
            "description": "Celebrating the establishment of Nepal as a sovereign Federal Democratic Republic."
        },
        {
            "month": 2, "day": 22, "isPublicHoliday": False,
            "titleNP": "विश्व वातावरण दिवस",
            "titleEN": "World Environment Day (June 5)",
            "category": "GLOBAL DAY",
            "description": "Global advocacy for green technology, sustainable development, and ecology."
        },
        {
            "month": 3, "day": 15, "isPublicHoliday": False,
            "titleNP": "राष्ट्रिय धान दिवस / दही चिउरा",
            "titleEN": "National Paddy Day (Asar 15)",
            "category": "FESTIVAL",
            "description": "Traditional celebration of rice planting in monsoon season with yogurt and flattened rice."
        },
        {
            "month": 3, "day": 25, "isPublicHoliday": False,
            "titleNP": "गुरु पूर्णिमा",
            "titleEN": "Guru Purnima",
            "category": "FESTIVAL",
            "description": "Sacred day of expressing reverence and gratitude to teachers, mentors, and educators."
        },
        {
            "month": 4, "day": 1, "isPublicHoliday": False,
            "titleNP": "साउने सङ्क्रान्ति",
            "titleEN": "Saune Sankranti",
            "category": "FESTIVAL",
            "description": "Beginning of the pious monsoon month of Shrawan and solstice transition."
        },
        {
            "month": 4, "day": 15, "isPublicHoliday": False,
            "titleNP": "खीर खाने दिन",
            "titleEN": "Kheer Khane Din",
            "category": "CULTURAL EVENT",
            "description": "Mid-monsoon culinary tradition of enjoying sweet and creamy rice pudding (Kheer)."
        },
        {
            "month": 4, "day": 24, "isPublicHoliday": True,
            "titleNP": "जनै पूर्णिमा / रक्षाबन्धन",
            "titleEN": "Janai Purnima & Rakshya Bandhan",
            "category": "PUBLIC HOLIDAY",
            "description": "Renewal of sacred holy thread (Janai) and tying protective Rakhi bands of brotherhood."
        },
        {
            "month": 4, "day": 25, "isPublicHoliday": False,
            "titleNP": "गाईजात्रा (Gai Jatra)",
            "titleEN": "Gai Jatra (Cow Festival)",
            "category": "FESTIVAL",
            "description": "Vibrant Newar carnival honoring deceased family souls with street humor, joy, and satire."
        },
        {
            "month": 4, "day": 31, "isPublicHoliday": True,
            "titleNP": "श्रीकृष्ण जन्माष्टमी",
            "titleEN": "Shree Krishna Janmashtami",
            "category": "PUBLIC HOLIDAY",
            "description": "National holiday celebrating the divine birth appearance of Lord Krishna with pujas."
        },
        {
            "month": 5, "day": 10, "isPublicHoliday": True,
            "titleNP": "हरितालिका तीज",
            "titleEN": "Haritalika Teej",
            "category": "PUBLIC HOLIDAY",
            "description": "Joyous Hindu women's fasting and cultural festival celebrated with red sarees, songs, and temple darshan."
        },
        {
            "month": 5, "day": 12, "isPublicHoliday": False,
            "titleNP": "ऋषि पञ्चमी",
            "titleEN": "Rishi Panchami",
            "category": "FESTIVAL",
            "description": "Spiritual worship of the Sapta Rishis for spiritual cleansing and purity."
        },
        {
            "month": 5, "day": 14, "isPublicHoliday": True,
            "titleNP": "गौरा पर्व",
            "titleEN": "Gaura Parva",
            "category": "PUBLIC HOLIDAY",
            "description": "Major festival of western Nepal celebrated with energetic Deuda songs and rituals."
        },
        {
            "month": 5, "day": 20, "isPublicHoliday": True,
            "titleNP": "इन्द्रजात्रा (Indra Jatra)",
            "titleEN": "Indra Jatra & Kumari Yatra",
            "category": "PUBLIC HOLIDAY",
            "description": "Grand chariot procession of Living Goddess Kumari and rain deity Lord Indra."
        },
        {
            "month": 5, "day": 22, "isPublicHoliday": False,
            "titleNP": "निजामती सेवा दिवस",
            "titleEN": "Civil Service Day",
            "category": "NATIONAL DAY",
            "description": "Commemorating the dedication of public administrators and civil servants."
        },
        {
            "month": 6, "day": 3, "isPublicHoliday": True,
            "titleNP": "संविधान दिवस",
            "titleEN": "Constitution Day (National Day)",
            "category": "PUBLIC HOLIDAY",
            "description": "National celebration marking the adoption of the Constitution of Nepal in 2072 BS."
        },
        {
            "month": 6, "day": 6, "isPublicHoliday": True,
            "titleNP": "घटस्थापना (Ghatasthapana)",
            "titleEN": "Ghatasthapana (Dashain Begins)",
            "category": "PUBLIC HOLIDAY",
            "description": "Inauguration of Navaratri with sacred Kalash installation and Jamara sowing."
        },
        {
            "month": 6, "day": 13, "isPublicHoliday": True,
            "titleNP": "फूलपाती (Fulpati)",
            "titleEN": "Fulpati (Maha Saptami)",
            "category": "PUBLIC HOLIDAY",
            "description": "Sacred arrival of auspicious plant offerings to historic royal palaces."
        },
        {
            "month": 6, "day": 14, "isPublicHoliday": True,
            "titleNP": "महाअष्टमी (Maha Ashtami)",
            "titleEN": "Maha Ashtami",
            "category": "PUBLIC HOLIDAY",
            "description": "Night of deep spiritual reflection and devotion to Goddess Durga Bhavani."
        },
        {
            "month": 6, "day": 15, "isPublicHoliday": True,
            "titleNP": "महानवमी (Maha Navami)",
            "titleEN": "Maha Navami",
            "category": "PUBLIC HOLIDAY",
            "description": "Worship of implements, vehicles, tools, and divine goddess aspects."
        },
        {
            "month": 6, "day": 16, "isPublicHoliday": True,
            "titleNP": "विजया दशमी (Bada Dashain)",
            "titleEN": "Vijaya Dashami (Bada Dashain)",
            "category": "PUBLIC HOLIDAY",
            "description": "Grand day of receiving auspicious red Tika, golden Jamara, and blessings from elders."
        },
        {
            "month": 6, "day": 17, "isPublicHoliday": True,
            "titleNP": "दशैं एकादशी (Papankusha)",
            "titleEN": "Dashain Ekadashi",
            "category": "PUBLIC HOLIDAY",
            "description": "Continuing family reunions, feasts, and high bamboo swings (Ping)."
        },
        {
            "month": 7, "day": 2, "isPublicHoliday": False,
            "titleNP": "काग तिहार (Kag Tihar)",
            "titleEN": "Kag Tihar (Crow Worship)",
            "category": "FESTIVAL",
            "description": "First day of Yamapanchak honoring crows as messengers of health and peace."
        },
        {
            "month": 7, "day": 3, "isPublicHoliday": False,
            "titleNP": "कुकुर तिहार (Kukur Tihar)",
            "titleEN": "Kukur Tihar (Dog Worship)",
            "category": "FESTIVAL",
            "description": "Honoring dogs for unconditional loyalty and companionship with garlands."
        },
        {
            "month": 7, "day": 4, "isPublicHoliday": True,
            "titleNP": "लक्ष्मी पूजा (Laxmi Puja)",
            "titleEN": "Laxmi Puja & Deepawali",
            "category": "PUBLIC HOLIDAY",
            "description": "Illuminating homes with oil lamps and honoring Goddess Laxmi for prosperity."
        },
        {
            "month": 7, "day": 5, "isPublicHoliday": True,
            "titleNP": "गोवर्धन पूजा / म्ह पूजा",
            "titleEN": "Govardhan Puja & Nepal Sambat",
            "category": "PUBLIC HOLIDAY",
            "description": "Nepal Sambat 1146 New Year, self-worship (Mha Puja), and Govardhan nature reverence."
        },
        {
            "month": 7, "day": 7, "isPublicHoliday": True,
            "titleNP": "भाइटीका (Bhai Tika)",
            "titleEN": "Bhai Tika (Tihar Finale)",
            "category": "PUBLIC HOLIDAY",
            "description": "Sisters adorn brothers with rainbow Tika and everlasting Makhamali garlands."
        },
        {
            "month": 7, "day": 11, "isPublicHoliday": True,
            "titleNP": "छठ पर्व (Chhath Parva)",
            "titleEN": "Chhath Parva (Sun God Festival)",
            "category": "PUBLIC HOLIDAY",
            "description": "Ancient Vedic riverbank festival worshipping the rising and setting Sun."
        },
        {
            "month": 8, "day": 3, "isPublicHoliday": False,
            "titleNP": "बाला चतुर्दशी",
            "titleEN": "Bala Chaturdashi",
            "category": "FESTIVAL",
            "description": "Sowing sacred Satbij seeds at Pashupatinath for eternal peace of ancestors."
        },
        {
            "month": 8, "day": 9, "isPublicHoliday": False,
            "titleNP": "विवाह पञ्चमी",
            "titleEN": "Vivah Panchami",
            "category": "FESTIVAL",
            "description": "Celebration of the divine marriage of Lord Ram and Goddess Sita in Janakpur."
        },
        {
            "month": 8, "day": 18, "isPublicHoliday": True,
            "titleNP": "उधौली पर्व / योमरी पुन्हि",
            "titleEN": "Udhauli Parva & Yomari Punhi",
            "category": "PUBLIC HOLIDAY",
            "description": "Kirat harvest thanksgiving and Newar sweet steamed Yomari dumpling feast."
        },
        {
            "month": 9, "day": 10, "isPublicHoliday": True,
            "titleNP": "क्रिसमस डे (Christmas)",
            "titleEN": "Christmas Day (Dec 25)",
            "category": "PUBLIC HOLIDAY",
            "description": "Global holiday celebrating love, charity, and the nativity of Jesus Christ."
        },
        {
            "month": 9, "day": 15, "isPublicHoliday": True,
            "titleNP": "तमु ल्होसार",
            "titleEN": "Tamu Lhosar (Gurung New Year)",
            "category": "PUBLIC HOLIDAY",
            "description": "Gurung community New Year celebrated with cultural parades and feasts."
        },
        {
            "month": 9, "day": 27, "isPublicHoliday": True,
            "titleNP": "राष्ट्रिय एकता दिवस / पृथ्वी जयन्ती",
            "titleEN": "National Unity Day (Prithvi Jayanti)",
            "category": "PUBLIC HOLIDAY",
            "description": "Commemorating the builder of unified modern Nepal, King Prithvi Narayan Shah."
        },
        {
            "month": 10, "day": 1, "isPublicHoliday": True,
            "titleNP": "माघे सङ्क्रान्ति / माघी",
            "titleEN": "Maghe Sankranti & Maghi",
            "category": "PUBLIC HOLIDAY",
            "description": "Winter festival of Chaku, Ghee, Tarul, and the Tharu New Year."
        },
        {
            "month": 10, "day": 6, "isPublicHoliday": True,
            "titleNP": "सोनाम ल्होसार",
            "titleEN": "Sonam Lhosar (Tamang New Year)",
            "category": "PUBLIC HOLIDAY",
            "description": "Tamang New Year festival marked with monastic prayers and family gatherings."
        },
        {
            "month": 10, "day": 10, "isPublicHoliday": False,
            "titleNP": "सरस्वती पूजा / वसन्त पञ्चमी",
            "titleEN": "Saraswati Puja (Vasant Panchami)",
            "category": "FESTIVAL",
            "description": "Worship of the Goddess of Wisdom, arts, creativity, and knowledge."
        },
        {
            "month": 10, "day": 16, "isPublicHoliday": False,
            "titleNP": "सहिद दिवस",
            "titleEN": "Martyrs' Day (Sahid Diwas)",
            "category": "NATIONAL DAY",
            "description": "Tribute to the courageous heroes who sacrificed their lives for freedom."
        },
        {
            "month": 11, "day": 3, "isPublicHoliday": True,
            "titleNP": "महाशिवरात्रि",
            "titleEN": "Maha Shivaratri",
            "category": "PUBLIC HOLIDAY",
            "description": "Grand night of Lord Shiva celebrated by sadhus and pilgrims at Pashupatinath."
        },
        {
            "month": 11, "day": 6, "isPublicHoliday": True,
            "titleNP": "ग्याल्पो ल्होसार",
            "titleEN": "Gyalpo Lhosar (Sherpa New Year)",
            "category": "PUBLIC HOLIDAY",
            "description": "Tibetan and Sherpa New Year celebrated with traditional Losar ceremonies."
        },
        {
            "month": 11, "day": 7, "isPublicHoliday": True,
            "titleNP": "राष्ट्रिय प्रजातन्त्र दिवस",
            "titleEN": "National Democracy Day",
            "category": "PUBLIC HOLIDAY",
            "description": "Commemorating the triumph of popular democracy in Nepal in 2007 BS."
        },
        {
            "month": 11, "day": 18, "isPublicHoliday": True,
            "titleNP": "फागु पूर्णिमा (Holi)",
            "titleEN": "Holi (Festival of Colors)",
            "category": "PUBLIC HOLIDAY",
            "description": "Vibrant festival of colours celebrating spring, friendship, and joy."
        },
        {
            "month": 11, "day": 24, "isPublicHoliday": True,
            "titleNP": "अन्तर्राष्ट्रिय महिला दिवस",
            "titleEN": "International Women's Day (Mar 8)",
            "category": "PUBLIC HOLIDAY",
            "description": "Celebrating women's empowerment, rights, and remarkable global achievements."
        },
        {
            "month": 12, "day": 4, "isPublicHoliday": False,
            "titleNP": "घोडेजात्रा (Ghode Jatra)",
            "titleEN": "Ghode Jatra (Horse Festival)",
            "category": "CULTURAL EVENT",
            "description": "Spectacular annual horse parades and equestrian stunts in Kathmandu valley."
        },
        {
            "month": 12, "day": 12, "isPublicHoliday": False,
            "titleNP": "चैते दशैं",
            "titleEN": "Chaite Dashain",
            "category": "FESTIVAL",
            "description": "Spring festivities worshipping Goddess Durga Bhavani."
        },
        {
            "month": 12, "day": 13, "isPublicHoliday": False,
            "titleNP": "राम नवमी",
            "titleEN": "Ram Navami",
            "category": "FESTIVAL",
            "description": "Celebration of the appearance day of Lord Ram."
        }
    ],

    2083: [
        {
            "month": 1, "day": 1, "isPublicHoliday": True,
            "titleNP": "नयाँ वर्ष (Nepali New Year)",
            "titleEN": "Nepali New Year 2083 & Bisket Jatra",
            "category": "PUBLIC HOLIDAY",
            "description": "Official national public holiday welcoming Bikram Sambat 2083 New Year."
        },
        {
            "month": 1, "day": 11, "isPublicHoliday": False,
            "titleNP": "लोकतन्त्र दिवस",
            "titleEN": "Loktantra Diwas (Democracy Day)",
            "category": "NATIONAL DAY",
            "description": "Commemorating the historic restoration of democratic parliament in Nepal."
        },
        {
            "month": 1, "day": 18, "isPublicHoliday": True,
            "titleNP": "अन्तर्राष्ट्रिय श्रमिक दिवस / बुद्ध जयन्ती",
            "titleEN": "Labour Day (May 1) & Buddha Jayanti",
            "category": "PUBLIC HOLIDAY",
            "description": "Auspicious alignment of International Labour Day and Gautama Buddha Jayanti / Ubhauli Parva."
        },
        {
            "month": 2, "day": 15, "isPublicHoliday": True,
            "titleNP": "गणतन्त्र दिवस",
            "titleEN": "Republic Day (Ganatantra Diwas)",
            "category": "PUBLIC HOLIDAY",
            "description": "National holiday celebrating Nepal's establishment as a Federal Republic."
        },
        {
            "month": 2, "day": 22, "isPublicHoliday": False,
            "titleNP": "विश्व वातावरण दिवस",
            "titleEN": "World Environment Day (June 5)",
            "category": "GLOBAL DAY",
            "description": "Global call for nature conservation, green tech, and climate sustainability."
        },
        {
            "month": 3, "day": 15, "isPublicHoliday": False,
            "titleNP": "राष्ट्रिय धान दिवस",
            "titleEN": "National Paddy Day (Dahi Chiura)",
            "category": "FESTIVAL",
            "description": "Traditional agricultural festival celebrating rice planting with curd and beaten rice."
        },
        {
            "month": 3, "day": 16, "isPublicHoliday": False,
            "titleNP": "गुरु पूर्णिमा",
            "titleEN": "Guru Purnima",
            "category": "FESTIVAL",
            "description": "Sacred day of gratitude dedicated to honoring teachers, mentors, and educators."
        },
        {
            "month": 4, "day": 1, "isPublicHoliday": False,
            "titleNP": "साउने सङ्क्रान्ति",
            "titleEN": "Saune Sankranti",
            "category": "FESTIVAL",
            "description": "Beginning of the holy monsoon month of Shrawan and solstice transition."
        },
        {
            "month": 4, "day": 15, "isPublicHoliday": False,
            "titleNP": "खीर खाने दिन",
            "titleEN": "Kheer Khane Din",
            "category": "CULTURAL EVENT",
            "description": "Mid-monsoon tradition of preparing and sharing sweet rice pudding (Kheer)."
        },
        {
            "month": 5, "day": 12, "isPublicHoliday": True,
            "titleNP": "जनै पूर्णिमा / रक्षाबन्धन",
            "titleEN": "Janai Purnima & Rakshya Bandhan",
            "category": "PUBLIC HOLIDAY",
            "description": "Sacred thread renewal and tying protective Rakhi bands of brotherhood."
        },
        {
            "month": 5, "day": 13, "isPublicHoliday": False,
            "titleNP": "गाईजात्रा (Gai Jatra)",
            "titleEN": "Gai Jatra (Cow Festival)",
            "category": "FESTIVAL",
            "description": "Vibrant Newar carnival honoring deceased souls with humor, satire, and joy."
        },
        {
            "month": 5, "day": 19, "isPublicHoliday": True,
            "titleNP": "श्रीकृष्ण जन्माष्टमी",
            "titleEN": "Krishna Janmashtami",
            "category": "PUBLIC HOLIDAY",
            "description": "National public holiday celebrating the birth of Lord Krishna with pujas."
        },
        {
            "month": 5, "day": 20, "isPublicHoliday": True,
            "titleNP": "गौरा पर्व",
            "titleEN": "Gaura Parva",
            "category": "PUBLIC HOLIDAY",
            "description": "Major regional festival of western Nepal celebrated with energetic Deuda songs."
        },
        {
            "month": 5, "day": 22, "isPublicHoliday": False,
            "titleNP": "निजामती सेवा दिवस",
            "titleEN": "Civil Service Day",
            "category": "NATIONAL DAY",
            "description": "Commemorating public administration and dedication of civil servants."
        },
        {
            "month": 5, "day": 29, "isPublicHoliday": True,
            "titleNP": "हरितालिका तीज",
            "titleEN": "Haritalika Teej",
            "category": "PUBLIC HOLIDAY",
            "description": "Joyous fasting festival of dance, song, and red sarees celebrated by women."
        },
        {
            "month": 5, "day": 31, "isPublicHoliday": False,
            "titleNP": "ऋषि पञ्चमी",
            "titleEN": "Rishi Panchami",
            "category": "FESTIVAL",
            "description": "Traditional worship of the Sapta Rishis for spiritual cleansing and purity."
        },
        {
            "month": 6, "day": 3, "isPublicHoliday": True,
            "titleNP": "संविधान दिवस",
            "titleEN": "Constitution Day (National Day)",
            "category": "PUBLIC HOLIDAY",
            "description": "National celebration marking the historic adoption of the Constitution of Nepal."
        },
        {
            "month": 6, "day": 9, "isPublicHoliday": True,
            "titleNP": "इन्द्रजात्रा (Indra Jatra)",
            "titleEN": "Indra Jatra & Kumari Yatra",
            "category": "PUBLIC HOLIDAY",
            "description": "Grand chariot procession of Living Goddess Kumari and rain deity Indra."
        },
        {
            "month": 6, "day": 25, "isPublicHoliday": True,
            "titleNP": "घटस्थापना (Ghatasthapana)",
            "titleEN": "Ghatasthapana (Dashain Begins)",
            "category": "PUBLIC HOLIDAY",
            "description": "First day of Dashain with sacred Kalash installation and Jamara sowing."
        },
        {
            "month": 7, "day": 1, "isPublicHoliday": True,
            "titleNP": "फूलपाती (Fulpati)",
            "titleEN": "Fulpati (Maha Saptami)",
            "category": "PUBLIC HOLIDAY",
            "description": "Sacred arrival of auspicious plant offerings to historic royal palaces."
        },
        {
            "month": 7, "day": 2, "isPublicHoliday": True,
            "titleNP": "महाअष्टमी (Maha Ashtami)",
            "titleEN": "Maha Ashtami",
            "category": "PUBLIC HOLIDAY",
            "description": "Night of deep spiritual reflection and devotion to Goddess Durga Bhavani."
        },
        {
            "month": 7, "day": 3, "isPublicHoliday": True,
            "titleNP": "महानवमी (Maha Navami)",
            "titleEN": "Maha Navami",
            "category": "PUBLIC HOLIDAY",
            "description": "Worship of Vishwakarma, craft implements, engines, and vehicles."
        },
        {
            "month": 7, "day": 4, "isPublicHoliday": True,
            "titleNP": "विजया दशमी (Bada Dashain)",
            "titleEN": "Vijaya Dashami (Bada Dashain)",
            "category": "PUBLIC HOLIDAY",
            "description": "Grand day of receiving red Tika, golden Jamara, and blessings from family elders."
        },
        {
            "month": 7, "day": 6, "isPublicHoliday": True,
            "titleNP": "दशैं एकादशी (Papankusha)",
            "titleEN": "Dashain Ekadashi",
            "category": "PUBLIC HOLIDAY",
            "description": "Continuing family reunions, feasts, and high bamboo swings (Ping)."
        },
        {
            "month": 7, "day": 21, "isPublicHoliday": False,
            "titleNP": "काग तिहार (Kag Tihar)",
            "titleEN": "Kag Tihar (Crow Worship)",
            "category": "FESTIVAL",
            "description": "First day of Yamapanchak honoring crows as messengers of health and peace."
        },
        {
            "month": 7, "day": 22, "isPublicHoliday": False,
            "titleNP": "कुकुर तिहार (Kukur Tihar)",
            "titleEN": "Kukur Tihar (Dog Worship)",
            "category": "FESTIVAL",
            "description": "Illuminating homes with oil lamps and honoring dogs for loyalty and protection."
        },
        {
            "month": 7, "day": 23, "isPublicHoliday": True,
            "titleNP": "लक्ष्मी पूजा (Laxmi Puja)",
            "titleEN": "Laxmi Puja & Deepawali",
            "category": "PUBLIC HOLIDAY",
            "description": "Illuminating homes with oil lamps and honoring Goddess Laxmi for prosperity."
        },
        {
            "month": 7, "day": 24, "isPublicHoliday": True,
            "titleNP": "गोवर्धन पूजा / म्ह पूजा",
            "titleEN": "Govardhan Puja & Nepal Sambat",
            "category": "PUBLIC HOLIDAY",
            "description": "Nepal Sambat 1147 New Year, self-worship (Mha Puja), and Govardhan reverence."
        },
        {
            "month": 7, "day": 25, "isPublicHoliday": True,
            "titleNP": "भाइटीका (Bhai Tika)",
            "titleEN": "Bhai Tika (Tihar Finale)",
            "category": "PUBLIC HOLIDAY",
            "description": "Sisters adorn brothers with rainbow Tika and everlasting Makhamali garlands."
        },
        {
            "month": 7, "day": 29, "isPublicHoliday": True,
            "titleNP": "छठ पर्व (Chhath Parva)",
            "titleEN": "Chhath Parva (Sun God Festival)",
            "category": "PUBLIC HOLIDAY",
            "description": "Ancient Vedic riverbank festival worshipping the rising and setting Sun."
        },
        {
            "month": 8, "day": 22, "isPublicHoliday": False,
            "titleNP": "बाला चतुर्दशी",
            "titleEN": "Bala Chaturdashi",
            "category": "FESTIVAL",
            "description": "Sowing sacred Satbij seeds at Pashupatinath for peace of ancestors."
        },
        {
            "month": 8, "day": 28, "isPublicHoliday": False,
            "titleNP": "विवाह पञ्चमी",
            "titleEN": "Vivah Panchami",
            "category": "FESTIVAL",
            "description": "Celebration of the divine marriage of Lord Ram and Goddess Sita in Janakpur."
        },
        {
            "month": 9, "day": 8, "isPublicHoliday": True,
            "titleNP": "उधौली पर्व / योमरी पुन्हि",
            "titleEN": "Udhauli Parva & Yomari Punhi",
            "category": "PUBLIC HOLIDAY",
            "description": "Kirat harvest thanksgiving and Newar sweet steamed Yomari dumpling feast."
        },
        {
            "month": 9, "day": 10, "isPublicHoliday": True,
            "titleNP": "क्रिसमस डे (Christmas)",
            "titleEN": "Christmas Day (Dec 25)",
            "category": "PUBLIC HOLIDAY",
            "description": "Global holiday celebrating love, charity, and the nativity of Jesus Christ."
        },
        {
            "month": 9, "day": 15, "isPublicHoliday": True,
            "titleNP": "तमु ल्होसार",
            "titleEN": "Tamu Lhosar (Gurung New Year)",
            "category": "PUBLIC HOLIDAY",
            "description": "Gurung community New Year celebrated with cultural parades and feasts."
        },
        {
            "month": 9, "day": 27, "isPublicHoliday": True,
            "titleNP": "राष्ट्रिय एकता दिवस / पृथ्वी जयन्ती",
            "titleEN": "National Unity Day (Prithvi Jayanti)",
            "category": "PUBLIC HOLIDAY",
            "description": "Commemorating the builder of unified modern Nepal, King Prithvi Narayan Shah."
        },
        {
            "month": 10, "day": 1, "isPublicHoliday": True,
            "titleNP": "माघे सङ्क्रान्ति / माघी",
            "titleEN": "Maghe Sankranti & Maghi",
            "category": "PUBLIC HOLIDAY",
            "description": "Winter festival of Chaku, Ghee, Tarul, and the Tharu New Year."
        },
        {
            "month": 10, "day": 16, "isPublicHoliday": False,
            "titleNP": "सहिद दिवस",
            "titleEN": "Martyrs' Day (Sahid Diwas)",
            "category": "NATIONAL DAY",
            "description": "Tribute to the courageous heroes who sacrificed their lives for freedom."
        },
        {
            "month": 10, "day": 24, "isPublicHoliday": True,
            "titleNP": "सोनाम ल्होसार",
            "titleEN": "Sonam Lhosar (Tamang New Year)",
            "category": "PUBLIC HOLIDAY",
            "description": "Tamang New Year festival marked with monastic prayers and family gatherings."
        },
        {
            "month": 10, "day": 28, "isPublicHoliday": False,
            "titleNP": "सरस्वती पूजा / वसन्त पञ्चमी",
            "titleEN": "Saraswati Puja (Vasant Panchami)",
            "category": "FESTIVAL",
            "description": "Worship of the Goddess of Wisdom, arts, creativity, and knowledge."
        },
        {
            "month": 11, "day": 7, "isPublicHoliday": True,
            "titleNP": "राष्ट्रिय प्रजातन्त्र दिवस",
            "titleEN": "National Democracy Day",
            "category": "PUBLIC HOLIDAY",
            "description": "Commemorating the triumph of popular democracy in Nepal in 2007 BS."
        },
        {
            "month": 11, "day": 22, "isPublicHoliday": True,
            "titleNP": "महाशिवरात्रि",
            "titleEN": "Maha Shivaratri",
            "category": "PUBLIC HOLIDAY",
            "description": "Grand night of Lord Shiva celebrated by sadhus and pilgrims at Pashupatinath."
        },
        {
            "month": 11, "day": 24, "isPublicHoliday": True,
            "titleNP": "अन्तर्राष्ट्रिय महिला दिवस",
            "titleEN": "International Women's Day (Mar 8)",
            "category": "PUBLIC HOLIDAY",
            "description": "Celebrating women's empowerment, rights, and remarkable global achievements."
        },
        {
            "month": 11, "day": 25, "isPublicHoliday": True,
            "titleNP": "ग्याल्पो ल्होसार",
            "titleEN": "Gyalpo Lhosar (Sherpa New Year)",
            "category": "PUBLIC HOLIDAY",
            "description": "Tibetan and Sherpa New Year celebrated with traditional Losar ceremonies."
        },
        {
            "month": 12, "day": 7, "isPublicHoliday": True,
            "titleNP": "फागु पूर्णिमा (Holi)",
            "titleEN": "Holi (Festival of Colors)",
            "category": "PUBLIC HOLIDAY",
            "description": "Vibrant festival of colours celebrating spring, friendship, and joy."
        },
        {
            "month": 12, "day": 24, "isPublicHoliday": False,
            "titleNP": "घोडेजात्रा (Ghode Jatra)",
            "titleEN": "Ghode Jatra (Horse Festival)",
            "category": "CULTURAL EVENT",
            "description": "Spectacular annual horse parades and equestrian stunts in Kathmandu valley."
        }
    ]
}

import urllib.request
import ssl
import re
import sys
import time
import datetime

COMMON_TRANSLATIONS = {
    "नयाँ वर्ष": "Nepali New Year",
    "लोकतन्त्र दिवस": "Democracy Day",
    "श्रमिक दिवस": "Labour Day (May 1)",
    "मजदुर दिवस": "Labour Day",
    "बुद्ध जयन्ती": "Buddha Jayanti",
    "उभौली": "Ubhauli Parva",
    "गणतन्त्र दिवस": "Republic Day",
    "वातावरण दिवस": "World Environment Day",
    "धान दिवस": "National Paddy Day",
    "दही चिउरा": "Dahi Chiura",
    "गुरु पूर्णिमा": "Guru Purnima",
    "साउने सङ्क्रान्ति": "Saune Sankranti",
    "खीर खाने दिन": "Kheer Khane Din",
    "जनै पूर्णिमा": "Janai Purnima",
    "रक्षाबन्धन": "Rakshya Bandhan",
    "गाईजात्रा": "Gai Jatra",
    "श्रीकृष्ण जन्माष्टमी": "Krishna Janmashtami",
    "गौरा पर्व": "Gaura Parva",
    "निजामती सेवा दिवस": "Civil Service Day",
    "हरितालिका तीज": "Haritalika Teej",
    "ऋषि पञ्चमी": "Rishi Panchami",
    "संविधान दिवस": "Constitution Day",
    "इन्द्रजात्रा": "Indra Jatra",
    "घटस्थापना": "Ghatasthapana",
    "फूलपाती": "Fulpati",
    "महाअष्टमी": "Maha Ashtami",
    "महानवमी": "Maha Navami",
    "विजया दशमी": "Vijaya Dashami (Bada Dashain)",
    "काग तिहार": "Kag Tihar",
    "कुकुर तिहार": "Kukur Tihar",
    "लक्ष्मी पूजा": "Laxmi Puja & Deepawali",
    "गोवर्धन पूजा": "Govardhan Puja",
    "म्ह पूजा": "Mha Puja",
    "भाइटीका": "Bhai Tika",
    "छठ पर्व": "Chhath Parva",
    "बाला चतुर्दशी": "Bala Chaturdashi",
    "विवाह पञ्चमी": "Vivah Panchami",
    "उधौली": "Udhauli Parva",
    "योमरी पुन्हि": "Yomari Punhi",
    "क्रिसमस डे": "Christmas Day",
    "तमु ल्होसार": "Tamu Lhosar",
    "पृथ्वी जयन्ती": "Prithvi Jayanti / National Unity Day",
    "माघे सङ्क्रान्ति": "Maghe Sankranti / Maghi",
    "सहिद दिवस": "Martyrs' Day",
    "सोनाम ल्होसार": "Sonam Lhosar",
    "सरस्वती पूजा": "Saraswati Puja / Vasant Panchami",
    "प्रजातन्त्र दिवस": "National Democracy Day",
    "महाशिवरात्रि": "Maha Shivaratri",
    "ग्याल्पो ल्होसार": "Gyalpo Lhosar",
    "महिला दिवस": "International Women's Day",
    "फागु पूर्णिमा": "Holi (Festival of Colors)",
    "घोडेजात्रा": "Ghode Jatra",
    "चैते दशैं": "Chaite Dashain",
    "राम नवमी": "Ram Navami"
}

def translate_festival_title(np_title):
    en_parts = []
    for np_key, en_val in COMMON_TRANSLATIONS.items():
        if np_key in np_title and en_val not in en_parts:
            en_parts.append(en_val)
    if en_parts:
        return " & ".join(en_parts)
    return np_title

def scrape_calendar_year(bs_year):
    ctx = ssl._create_unverified_context()
    headers = {'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64)'}
    nepali_digits = {'०':'0','१':'1','२':'2','३':'3','४':'4','५':'5','६':'6','७':'7','८':'8','९':'9'}
    
    all_events = []
    print(f"Scraping live calendar data for BS {bs_year}...")
    
    for month in range(1, 13):
        url = f"https://nepalicalendar.rat32.com/index_nep.php?year={bs_year}&month={month}"
        try:
            req = urllib.request.Request(url, headers=headers)
            with urllib.request.urlopen(req, context=ctx, timeout=20) as res:
                html = res.read().decode('utf-8', errors='ignore')
        except Exception as e:
            print(f"  [!] Month {month} fetch error: {e}")
            continue

        blocks = re.findall(r'(<div id="nday">.*?)(?=<div id="nday">|</body>|</html>)', html, re.DOTALL)
        for b in blocks:
            nday_m = re.search(r'<div id="nday">(.*?)</div>', b, re.DOTALL)
            fest_m = re.search(r'<div id="fest">(.*?)</div>', b, re.DOTALL)
            dashi_m = re.search(r'<div id="dashi">(.*?)</div>', b, re.DOTALL)
            
            if not nday_m:
                continue
            
            nday_raw = nday_m.group(1)
            is_red = any(r in nday_raw.lower() for r in ['color="red"', "color='red'", 'color: red', 'color:red'])
            
            nday_clean = re.sub(r'<[^>]+>', '', nday_raw).strip()
            day_str = "".join(nepali_digits.get(c, c) for c in nday_clean if c in nepali_digits or c.isdigit())
            if not day_str:
                continue
            day_num = int(day_str)
            
            fest_clean = ""
            if fest_m:
                fest_clean = re.sub(r'<[^>]+>', ' ', fest_m.group(1)).strip()
                fest_clean = " ".join(fest_clean.split()).replace('&nbsp;', ' ').strip()

            tithi_clean = ""
            if dashi_m:
                tithi_clean = re.sub(r'<[^>]+>', ' ', dashi_m.group(1)).strip().replace('&nbsp;', '').strip()

            # Ignore empty Saturday weekends without festival
            if not fest_clean:
                continue

            is_public_holiday = is_red

            # Categorization
            cat = "FESTIVAL"
            if is_public_holiday:
                cat = "PUBLIC HOLIDAY"
            elif any(w in fest_clean for w in ["दिवस", "Day", "जयन्ती", "एकता"]):
                cat = "NATIONAL DAY"
            elif any(w in fest_clean for w in ["यात्रा", "Jatra", "कार्म", "परेड"]):
                cat = "CULTURAL EVENT"

            en_title = translate_festival_title(fest_clean)
            desc = f"{fest_clean} (Tithi: {tithi_clean})" if tithi_clean else fest_clean

            all_events.append({
                "month": month,
                "day": day_num,
                "isPublicHoliday": is_public_holiday,
                "titleNP": fest_clean,
                "titleEN": en_title,
                "category": cat,
                "description": desc
            })
        
        time.sleep(0.3)
        
    print(f"  [+] Scraped {len(all_events)} events for BS {bs_year}.")
    return all_events

def get_current_bs_year():
    today = datetime.date.today()
    # Bikram Sambat new year begins around April 14
    if (today.month, today.day) >= (4, 14):
        return today.year + 57
    else:
        return today.year + 56

def main():
    script_dir = os.path.dirname(os.path.abspath(__file__))
    output_dir = script_dir if os.path.basename(script_dir) == 'data' else os.path.join(script_dir, 'data')
    os.makedirs(output_dir, exist_ok=True)

    # 1. First generate all hand-curated years (high precision descriptions & translations)
    for year, holidays in years_data.items():
        filepath = os.path.join(output_dir, f'holidays_{year}.json')
        data = {
            "year": year,
            "count": len(holidays),
            "holidays": holidays
        }
        with open(filepath, 'w', encoding='utf-8') as f:
            json.dump(data, f, ensure_ascii=False, indent=2)
        print(f"Generated curated dataset: {filepath} ({len(holidays)} events)")

    # 2. Check if auto-update / scraping is requested or running in CI
    args = sys.argv[1:]
    scrape_years = []
    
    if "--auto" in args or "--all" in args:
        curr_bs = get_current_bs_year()
        # Ensure current year and next year are present
        for y in [curr_bs, curr_bs + 1]:
            target_file = os.path.join(output_dir, f'holidays_{y}.json')
            if not os.path.exists(target_file) or os.path.getsize(target_file) < 500:
                scrape_years.append(y)
    else:
        for arg in args:
            if arg.isdigit() and int(arg) > 2070:
                scrape_years.append(int(arg))

    # 3. Scrape any missing or requested years
    for y in set(scrape_years):
        filepath = os.path.join(output_dir, f'holidays_{y}.json')
        if y in years_data:
            print(f"Year {y} already curated.")
            continue
        events = scrape_calendar_year(y)
        if events:
            data = {
                "year": y,
                "count": len(events),
                "holidays": events
            }
            with open(filepath, 'w', encoding='utf-8') as f:
                json.dump(data, f, ensure_ascii=False, indent=2)
            print(f"Generated scraped dataset: {filepath} ({len(events)} events)")
        else:
            print(f"Could not scrape data for year {y} (calendar may not yet be published).")

if __name__ == "__main__":
    main()
