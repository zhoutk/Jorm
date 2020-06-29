/*
 Navicat Premium Data Transfer

 Source Server         : pg
 Source Server Type    : PostgreSQL
 Source Server Version : 110007
 Source Host           : localhost:5432
 Source Catalog        : Jorm
 Source Schema         : public

 Target Server Type    : PostgreSQL
 Target Server Version : 110007
 File Encoding         : 65001

 Date: 29/06/2020 21:32:37
*/


-- ----------------------------
-- Table structure for users
-- ----------------------------
DROP TABLE IF EXISTS "public"."users";
CREATE TABLE "public"."users" (
  "id" int4 NOT NULL DEFAULT nextval('users_id_seq'::regclass),
  "username" varchar(256) COLLATE "pg_catalog"."default",
  "password" varchar(256) COLLATE "pg_catalog"."default",
  "update_time" varchar(32) COLLATE "pg_catalog"."default" DEFAULT now()
)
;

-- ----------------------------
-- Records of users
-- ----------------------------
INSERT INTO "public"."users" VALUES (2, 'robin', '4123560', '2020-06-15 23:17:30');
INSERT INTO "public"."users" VALUES (5, 'john', '567', '2020-06-16 09:34:42');
INSERT INTO "public"."users" VALUES (7, 'marry', '123', '2020-06-16 10:19:17');
INSERT INTO "public"."users" VALUES (11, '李四', '芝麻开门', '2020-06-16 10:29:08');
INSERT INTO "public"."users" VALUES (1, '张三', '123', '2020-06-29 21:30:40');

-- ----------------------------
-- Primary Key structure for table users
-- ----------------------------
ALTER TABLE "public"."users" ADD CONSTRAINT "users_pkey" PRIMARY KEY ("id");
