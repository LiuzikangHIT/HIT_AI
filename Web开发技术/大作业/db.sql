PGDMP  )                     |            forum    16.2    16.2                 0    0    ENCODING    ENCODING        SET client_encoding = 'UTF8';
                      false                       0    0 
   STDSTRINGS 
   STDSTRINGS     (   SET standard_conforming_strings = 'on';
                      false                       0    0 
   SEARCHPATH 
   SEARCHPATH     8   SELECT pg_catalog.set_config('search_path', '', false);
                      false                       1262    16415    forum    DATABASE     �   CREATE DATABASE forum WITH TEMPLATE = template0 ENCODING = 'UTF8' LOCALE_PROVIDER = libc LOCALE = 'Chinese (Simplified)_China.936';
    DROP DATABASE forum;
                postgres    false            �            1259    16445    administrators    TABLE     �   CREATE TABLE public.administrators (
    adminname character varying(10) NOT NULL,
    password character varying(10) NOT NULL
);
 "   DROP TABLE public.administrators;
       public         heap    postgres    false            �            1259    16462    comments    TABLE     �   CREATE TABLE public.comments (
    commentid character varying(5) NOT NULL,
    content character varying(200) NOT NULL,
    postid character varying(5) NOT NULL,
    username character varying(10) NOT NULL
);
    DROP TABLE public.comments;
       public         heap    postgres    false            �            1259    16450    modules    TABLE     {   CREATE TABLE public.modules (
    moduleid character varying(5) NOT NULL,
    modulename character varying(10) NOT NULL
);
    DROP TABLE public.modules;
       public         heap    postgres    false            �            1259    16455    posts    TABLE     �   CREATE TABLE public.posts (
    username character varying(10),
    content character varying(200),
    likecount integer,
    postid character varying(5) NOT NULL,
    moduleid character varying(5)
);
    DROP TABLE public.posts;
       public         heap    postgres    false            �            1259    16416    users    TABLE     x   CREATE TABLE public.users (
    username character varying(10) NOT NULL,
    password character varying(10) NOT NULL
);
    DROP TABLE public.users;
       public         heap    postgres    false            �          0    16445    administrators 
   TABLE DATA           =   COPY public.administrators (adminname, password) FROM stdin;
    public          postgres    false    216   z       �          0    16462    comments 
   TABLE DATA           H   COPY public.comments (commentid, content, postid, username) FROM stdin;
    public          postgres    false    219   �       �          0    16450    modules 
   TABLE DATA           7   COPY public.modules (moduleid, modulename) FROM stdin;
    public          postgres    false    217   �       �          0    16455    posts 
   TABLE DATA           O   COPY public.posts (username, content, likecount, postid, moduleid) FROM stdin;
    public          postgres    false    218   �       �          0    16416    users 
   TABLE DATA           3   COPY public.users (username, password) FROM stdin;
    public          postgres    false    215   v       c           2606    16449 "   administrators administrators_pkey 
   CONSTRAINT     g   ALTER TABLE ONLY public.administrators
    ADD CONSTRAINT administrators_pkey PRIMARY KEY (adminname);
 L   ALTER TABLE ONLY public.administrators DROP CONSTRAINT administrators_pkey;
       public            postgres    false    216            i           2606    16466    comments comments_pkey 
   CONSTRAINT     [   ALTER TABLE ONLY public.comments
    ADD CONSTRAINT comments_pkey PRIMARY KEY (commentid);
 @   ALTER TABLE ONLY public.comments DROP CONSTRAINT comments_pkey;
       public            postgres    false    219            e           2606    16461    modules modules_pkey 
   CONSTRAINT     X   ALTER TABLE ONLY public.modules
    ADD CONSTRAINT modules_pkey PRIMARY KEY (moduleid);
 >   ALTER TABLE ONLY public.modules DROP CONSTRAINT modules_pkey;
       public            postgres    false    217            g           2606    16459    posts posts_pkey 
   CONSTRAINT     R   ALTER TABLE ONLY public.posts
    ADD CONSTRAINT posts_pkey PRIMARY KEY (postid);
 :   ALTER TABLE ONLY public.posts DROP CONSTRAINT posts_pkey;
       public            postgres    false    218            a           2606    16436    users users_pkey 
   CONSTRAINT     T   ALTER TABLE ONLY public.users
    ADD CONSTRAINT users_pkey PRIMARY KEY (username);
 :   ALTER TABLE ONLY public.users DROP CONSTRAINT users_pkey;
       public            postgres    false    215            �      x��ͩ�4426����� /�      �   �   x�3�|�1���5Ovt>ٱ�����f��d�ӾΧ�6?���iș�S�eR�����K���y���ټ9@�/�4�X��t�d��M_���i�Y\Y�eV߾��k�]X�ięS��e��������Ovv���Y�tr��S��^�t��';f=]����eϦ�L*M�ܔ���/��Z�dG=O',޹�鮩P3�8A��"�TqIiJj^	W� Ņ��      �   L   x�A ��1	二手闲置
2	打听求助
3	恋爱交友
4	兼职招聘
\.


У#6      �   �   x�v ��sys	收二手自行车	4	2	1
mlx	求助，谁能帮我改bug	15	3	2
lzk	哦耶，终于做完大作业了	11	1	3
\.


*3><      �      x�+�,�442�ʩ��&�f\1z\\\ N�J     