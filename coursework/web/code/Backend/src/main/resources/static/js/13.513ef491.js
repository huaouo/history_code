(window["webpackJsonp"]=window["webpackJsonp"]||[]).push([[13],{i0i3:function(t,s,o){"use strict";o.r(s);var a=function(){var t=this,s=t.$createElement,o=t._self._c||s;return o("div",{staticClass:"app flex-row align-items-center"},[o("notifications",{attrs:{group:"notify"}}),o("div",{staticClass:"container"},[o("b-row",{staticClass:"justify-content-center"},[o("b-col",{attrs:{md:"8"}},[o("b-card-group",[o("b-card",{staticClass:"p-4",attrs:{"no-body":""}},[o("b-card-body",[o("b-form",[o("h1",[t._v("用户登录")]),o("b-input-group",{staticClass:"mb-3"},[o("b-input-group-prepend",[o("b-input-group-text",[o("i",{staticClass:"icon-user"})])],1),o("b-form-input",{staticClass:"form-control",attrs:{state:t.idState,type:"text",placeholder:"用户名",autocomplete:"username email"},model:{value:t.id,callback:function(s){t.id=s},expression:"id"}}),o("b-form-invalid-feedback",[t._v("\n                    请输入用户名!\n                  ")])],1),o("b-input-group",{staticClass:"mb-4"},[o("b-input-group-prepend",[o("b-input-group-text",[o("i",{staticClass:"icon-lock"})])],1),o("b-form-input",{staticClass:"form-control",attrs:{state:t.passwordState,type:"password",placeholder:"密码",autocomplete:"current-password"},model:{value:t.password,callback:function(s){t.password=s},expression:"password"}}),o("b-form-invalid-feedback",[t._v("\n                    密码长度必须在6-50位之间!\n                  ")])],1),o("b-row",[o("b-col",{staticClass:"text-center"},[o("b-button",{staticClass:"px-4",attrs:{variant:"primary"},on:{click:t.submit}},[t._v("登录")])],1)],1)],1)],1)],1),o("b-card",{staticClass:"text-white bg-primary py-5 d-md-down-none",staticStyle:{width:"44%"},attrs:{"no-body":""}},[o("b-card-body",{staticClass:"text-center"},[o("div",[o("h2",[t._v("学生成绩管理系统")]),o("p",[t._v("欢迎使用学生成绩管理系统。")]),o("p",[t._v("杨振华"),o("br"),t._v("软件工程1603"),o("br"),t._v("201626811225")]),o("p")])])],1)],1)],1)],1)],1)],1)},i=[],e=o("vDqi"),r={name:"Login",data:function(){return{id:"",password:""}},computed:{idState:function(){return this.id.length>0},passwordState:function(){return this.password.length>=6&&this.password.length<=50}},methods:{submit:function(){var t=this;this.idState&&this.passwordState?e({method:"post",url:"/controller/login",data:{id:this.id,password:this.password}}).then(function(s){window.loggedStatus=s.data,"ILLEGAL"!==window.loggedStatus?t.$router.push("/dashboard"):t.$notify({group:"notify",type:"error",title:"错误",text:"用户名或密码错误, 请重新输入!"})}).catch(function(s){console.log(s),t.$notify({group:"notify",type:"error",title:"错误",text:"网络错误!"})}):this.$notify({group:"notify",type:"error",title:"错误",text:"用户名或密码不满足条件!"})}}},n=r,c=o("KHd+"),d=Object(c["a"])(n,a,i,!1,null,null,null);s["default"]=d.exports}}]);
//# sourceMappingURL=13.513ef491.js.map