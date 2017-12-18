var opt = {
    getDoms : function(){
        this.doms = {
            role : $('.role'),
            gunzi : $('.gunzi'),
            body:$('body'),
            shatan : $('.shatan'),
            length : $('#length')
        }
    },
    init : function(number){
        var _self = this;
        _self.number = number;
        _self.getDoms();
        _self.addEvent();
    },
    addEvent : function(){
        var _slef = this;

            _slef.replaceRole(_slef,_slef.biaoqiang);

    },
    replaceRole : function(_slef,callbacl){
        var s = 1;
        var cler = ''
        cler = setInterval(function(){
            s++;
            _slef.doms.role.find('img').attr('src','img/role'+s+'.png')
            if(s==4){
                clearInterval(cler);
                callbacl&&callbacl(_slef);
            }
        },100)

    },
    biaoqiang : function(_slef){
        _slef.doms.gunzi.css('display','block').animate({
            top:100
        },2000);
        var timeseed = 0;
        var rotate = 160;
        var clarrotate = '';
        var clarseed = '';
        clarrotate = setInterval(function(){
            rotate++
            _slef.doms.gunzi.css('transform','rotate('+rotate+'deg)')
        },100);
        _slef.doms.gunzi.css('display','block').animate({
           top:300
        },2300,function(){
           clearInterval(clarrotate);
            clearInterval(clarseed);
            if(window.nandu==1){
                    if(window.scores>100){
                        alert('恭喜你过关了');
                        window.setgameover = 1;
                    }else{
                        alert('很可惜~你没有过关')
                        window.setgameover = 1;
                    }
            }
            if(window.nandu==2){
                if(window.scores>400){
                    alert('恭喜你过关了');
                    window.setgameover = 1;
                }else{
                    alert('很可惜~你没有过关');
                    window.setgameover = 1;
                }
            }
            if(window.nandu==3){
                if(window.scores>800){
                    alert('恭喜你过关了');
                    window.setgameover = 1;
                }else{
                    alert('很可惜~你没有过关');
                    window.setgameover = 1;
                }
            }
            window.location.reload(true)
        });

        _slef.doms.shatan.animate({
            width : $('body').width(),
            marginLeft : 0
        },2000);
        _slef.doms.role.animate({
            left : -200
        },3000);
        clarseed = setInterval(function(){
            timeseed--;
            _slef.doms.shatan.css('background','url(img/shatan.png) repeat-x '+timeseed+'px 210px');
            _slef.doms.length.html( Math.abs(timeseed));
            window.scores = ( Math.abs(timeseed));
            _slef.doms.body.css('background','url(img/bg_rep.png) repeat-x '+timeseed+'px  0')
        },_slef.number);
    }

};
var i=0;
window.nandu = 2
function fire(number){
    var number = parseInt(number);
    number = number/2;
    number = 100 - number;
    if(i==0) {
        window.setgameover = 0;
        opt.init(number);
    }
    i++
}

function getGameType(){
    return '标枪'
}
function getScore (){
    return window.scores
}


function getNandu(){
	if (window.nandu == 1) {
		return '高'
	} else if (window.nandu == 2) {
		return '中'
	}

    return '低'
}

function Checkpoint1(){
    window.nandu = 1
}
function Checkpoint2(){
    window.nandu = 2
}
function Checkpoint3(){
    window.nandu =3
}

window.setgameover = 2
function getGameOver (){
    return window.setgameover
}
function getStartGame (){
    return 1
}